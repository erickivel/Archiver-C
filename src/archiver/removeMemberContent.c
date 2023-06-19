#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void removeMemberContent(struct Archiver *archiver, int memberIndex) {
  FILE *archiverRead = fopen(archiver->pathName, "rb");
  FILE *archiverWrite = fopen(archiver->pathName, "r+b");

  if (!archiverRead || !archiverWrite) {
    perror("It was not possible to open the archiver files!");
    exit(1);
  }

  struct MemberInfo memberToDelete =
      archiver->directory.membersInfo[memberIndex];
  struct MemberInfo *allMembers = archiver->directory.membersInfo;

  char buffer[BUFFER_SIZE];

  fseek(archiverWrite, memberToDelete.startPosition, SEEK_SET);
  fseek(archiverRead, memberToDelete.startPosition + memberToDelete.size,
        SEEK_SET);

  // Move backward all members content after the member content deleted
  for (int i = memberIndex; i < archiver->directory.numMembers - 1; i++) {

    int iterations = allMembers[i + 1].size / BUFFER_SIZE;
    int bytesLeft = allMembers[i + 1].size % BUFFER_SIZE;

    for (int j = 0; j < iterations; j++) {
      fread(buffer, sizeof(char), BUFFER_SIZE, archiverRead);
      fwrite(buffer, sizeof(char), BUFFER_SIZE, archiverWrite);
    }

    fread(buffer, sizeof(char), bytesLeft, archiverRead);
    fwrite(buffer, sizeof(char), bytesLeft, archiverWrite);

    // Update members and directory start position
    archiver->directory.membersInfo[i + 1].startPosition -= memberToDelete.size;
  }
  archiver->directory.startPosition -= memberToDelete.size;
  truncate(archiver->pathName, archiver->directory.startPosition - 1);

  fclose(archiverRead);
  fclose(archiverWrite);
}
