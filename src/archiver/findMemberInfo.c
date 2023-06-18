#include "../archiver.h"
#include <string.h>

struct MemberInfo *findMemberInfo(struct Archiver *archiver, char *filePath,
                                  int *memberIdx) {
  *memberIdx = -1;
  for (int i = 0; i < archiver->directory.numMembers; i++) {
    if (!strcmp(archiver->directory.membersInfo[i].pathName, filePath)) {
      *memberIdx = i;
      return &archiver->directory.membersInfo[i];
    }
  }

  return NULL;
}
