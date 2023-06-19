#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct MemberInfo *createMemberInfo(char *filePath) {
  struct MemberInfo *memberInfo = malloc(sizeof(struct MemberInfo));
  struct stat fileInfo;
  stat(filePath, &fileInfo);

  memberInfo->index = 0;
  memberInfo->pathNameLen = strlen(filePath);
  memberInfo->pathName = malloc(sizeof(char) * (strlen(filePath) + 1));
  strcpy(memberInfo->pathName, filePath);
  memberInfo->startPosition = 0;
  memberInfo->userID = fileInfo.st_uid;
  memberInfo->permissions = fileInfo.st_mode;
  memberInfo->size = fileInfo.st_size;
  memberInfo->updatedAt = fileInfo.st_mtime;

  return memberInfo;
}

void archiverInsert(struct Archiver *archiver, struct FilePaths *filePaths) {
  for (int i = 0; i < filePaths->size; i++) {
    int memberIndex;
    struct MemberInfo *memberInfo =
        findMemberInfo(archiver, filePaths->names[i], &memberIndex);

    if (memberInfo) {
      removeMemberContent(archiver, memberIndex);

      struct MemberInfo memberData = *memberInfo;
      // Update member & archiver directory (ordenate by start position)
      for (int j = memberIndex; j < archiver->directory.numMembers - 1; j++) {
        archiver->directory.membersInfo[j] =
            archiver->directory.membersInfo[j + 1];
      }

      // Update member size
      struct stat st;
      stat(memberData.pathName, &st);
      memberData.size = st.st_size;

      archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
          memberData;

      appendMemberContent(
          archiver,
          &archiver->directory.membersInfo[archiver->directory.numMembers - 1]);

    } else {
      memberInfo = createMemberInfo(filePaths->names[i]);

      // Insert file
      appendMemberContent(archiver, memberInfo);

      // Update directory and member info
      archiver->directory.numMembers++;
      archiver->directory.membersInfo =
          realloc(archiver->directory.membersInfo,
                  sizeof(struct MemberInfo) * archiver->directory.numMembers);
      archiver->directory.size =
          sizeof(size_t) + sizeof(int) +
          archiver->directory.numMembers * sizeof(struct MemberInfo);
      memberInfo->index = archiver->directory.numMembers;
      archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
          *memberInfo;

      free(memberInfo);
    }
  }

  writeDirectoryOnFile(archiver);
}
