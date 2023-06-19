#include "../archiver.h"
#include <stdio.h>
#include <sys/stat.h>

void archiverUpdate(struct Archiver *archiver, struct FilePaths *filePaths) {
  for (int i = 0; i < filePaths->size; i++) {
    int memberIndex;
    struct MemberInfo *memberInfo =
        findMemberInfo(archiver, filePaths->names[i], &memberIndex);

    if (!memberInfo) {
      printf("The file '%s' doesn't exist on the archiver '%s'\n",
             filePaths->names[i], archiver->pathName);
      return;
    }

    struct MemberInfo memberData = *memberInfo;

    struct stat memberToUpdateStat;
    stat(memberInfo->pathName, &memberToUpdateStat);

    // If member to insert update date is older do not update
    if (memberToUpdateStat.st_mtime <= memberData.updatedAt) {
      printf("'%s' was not updated since the last time it was archived\n",
             filePaths->names[i]);
      return;
    }

    printf("Updating: %s\n", memberData.pathName);

    removeMemberContent(archiver, memberIndex);

    // Update member & archiver directory (ordenate by start position)
    for (int j = memberIndex; j < archiver->directory.numMembers - 1; j++) {
      archiver->directory.membersInfo[j] =
          archiver->directory.membersInfo[j + 1];
    }

    // Update member size and update date
    memberData.size = memberToUpdateStat.st_size;
    memberData.updatedAt = memberToUpdateStat.st_mtime;
    archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
        memberData;

    appendMemberContent(
        archiver,
        &archiver->directory.membersInfo[archiver->directory.numMembers - 1]);
  }

  writeDirectoryOnFile(archiver);
}
