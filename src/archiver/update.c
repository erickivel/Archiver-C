#include "../archiver.h"
#include <sys/stat.h>

void archiverUpdate(struct Archiver *archiver, struct FilePaths *filePaths) {
  for (int i = 0; i < filePaths->size; i++) {
    int memberIndex;
    struct MemberInfo *memberInfo =
        findMemberInfo(archiver, filePaths->names[i], &memberIndex);

    if (!memberInfo)
      return;

    struct MemberInfo memberData = *memberInfo;

    struct stat memberToUpdateStat;
    stat(memberInfo->pathName, &memberToUpdateStat);

    if (memberToUpdateStat.st_mtime <= memberData.updatedAt)
      return;

    removeMemberContent(archiver, memberIndex);

    // Update member & archiver directory (ordenate by start position)
    for (int j = memberIndex; j < archiver->directory.numMembers - 1; j++) {
      archiver->directory.membersInfo[j] =
          archiver->directory.membersInfo[j + 1];
    }

    // Update member size
    memberData.size = memberToUpdateStat.st_size;
    archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
        memberData;

    appendMemberContent(
        archiver,
        &archiver->directory.membersInfo[archiver->directory.numMembers - 1]);
  }

  writeDirectoryOnFile(archiver);
}
