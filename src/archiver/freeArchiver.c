#include "../archiver.h"
#include <stdlib.h>

void freeArchiver(struct Archiver *archiver) {
  free(archiver->pathName);

  for (int i = 0; i < archiver->directory.numMembers; i++) {
    free(archiver->directory.membersInfo[i].pathName);
  }
  free(archiver->directory.membersInfo);

  free(archiver);
}
