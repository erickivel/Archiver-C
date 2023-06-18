#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>

void appendMemberContent(struct Archiver *archiver,
                         struct MemberInfo *memberInfo) {

  FILE *archiverFile = fopen(archiver->pathName, "r+b");
  FILE *fileToInsert = fopen(memberInfo->pathName, "rb");

  if (!archiverFile || !fileToInsert) {
    perror("It was not possible to open the archiver files!");
    exit(1);
  }

  char buffer[BUFFER_SIZE];
  int iterations = memberInfo->size / BUFFER_SIZE;
  int bytesLeft = memberInfo->size % BUFFER_SIZE;

  fseek(archiverFile, archiver->directory.startPosition, SEEK_SET);

  for (int i = 0; i < iterations; i++) {
    fread(buffer, sizeof(char), BUFFER_SIZE, fileToInsert);
    fwrite(buffer, sizeof(char), BUFFER_SIZE, archiverFile);
  }

  fread(buffer, sizeof(char), bytesLeft, fileToInsert);
  fwrite(buffer, sizeof(char), bytesLeft, archiverFile);

  // Update directory and member start position
  memberInfo->startPosition = archiver->directory.startPosition;
  archiver->directory.startPosition = ftell(archiverFile);

  fclose(archiverFile);
  fclose(fileToInsert);
}
