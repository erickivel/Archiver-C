#include "../archiver.h"
#include <stdio.h>

void archiverMove(struct Archiver *archiver, char *targetFilePath,
                  struct FilePaths *filePaths) {

  int targetMemberPos;
  struct MemberInfo *targetMember =
      findMemberInfo(archiver, targetFilePath, &targetMemberPos);

  int memberToMovePos;
  struct MemberInfo *memberToMove =
      findMemberInfo(archiver, filePaths->names[0], &memberToMovePos);

  if (!targetMember) {
    printf("The file '%s' doesn't exist on the archiver '%s'\n", targetFilePath,
           archiver->pathName);
    return;
  }

  if (!memberToMove) {
    printf("The file '%s' doesn't exist on the archiver '%s'\n",
           filePaths->names[0], archiver->pathName);
    return;
  }

  int targetMemberOrder = targetMember->index;
  int memberToMoveOrder = memberToMove->index;

  // Update members index
  if (targetMemberOrder > memberToMoveOrder) {
    memberToMove->index = targetMemberOrder;

    for (int j = 0; j < archiver->directory.numMembers; j++) {
      struct MemberInfo *currMember = &archiver->directory.membersInfo[j];

      if (currMember != memberToMove) {
        if (currMember->index > memberToMoveOrder &&
            currMember->index <= targetMemberOrder)
          currMember->index--;
      }
    }
  } else if (targetMemberOrder < memberToMoveOrder) {
    memberToMove->index = targetMemberOrder + 1;

    for (int j = 0; j < archiver->directory.numMembers; j++) {
      struct MemberInfo *currMember = &archiver->directory.membersInfo[j];

      if (currMember != memberToMove) {
        if (currMember->index < memberToMoveOrder &&
            currMember->index > targetMemberOrder)
          currMember->index++;
      }
    }
  }

  writeDirectoryOnFile(archiver);
}
