#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void archiverRemove(struct Archiver *archiver, struct FilePaths *filePaths) {
  for (int i = 0; i < filePaths->size; i++) {
    int memberIndex;
    struct MemberInfo *memberInfo =
        findMemberInfo(archiver, filePaths->names[i], &memberIndex);

    if (!memberInfo) {
      printf("The file '%s' doesn't exist on the archiver '%s'\n",
             filePaths->names[i], archiver->pathName);
      return;
    }

    int memberOrder = memberInfo->index;

    removeMemberContent(archiver, memberIndex);

    free(memberInfo->pathName);

    // Update content (ordenate by start position)
    for (int j = memberIndex; j < archiver->directory.numMembers - 1; j++) {
      archiver->directory.membersInfo[j] =
          archiver->directory.membersInfo[j + 1];
    }

    archiver->directory.numMembers--;
    // Update members index
    for (int j = 0; j < archiver->directory.numMembers; j++) {
      if (archiver->directory.membersInfo[j].index > memberOrder)
        archiver->directory.membersInfo[j].index--;
    }

    // Update directory info
    archiver->directory.size -= sizeof(struct MemberInfo);
    archiver->directory.membersInfo =
        realloc(archiver->directory.membersInfo,
                sizeof(struct MemberInfo) * archiver->directory.numMembers);
  }

  writeDirectoryOnFile(archiver);
}
