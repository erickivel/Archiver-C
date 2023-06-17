#include "../archiver.h"
#include <stdio.h>
#include <string.h>

void writeDirectoryOnFile(struct Archiver *archiver) {
  FILE *archiverFile = fopen(archiver->pathName, "r+b");
  struct Directory directory = archiver->directory;

  // Write directory info
  fwrite(&directory.startPosition, sizeof(size_t), 1, archiverFile);

  fseek(archiverFile, directory.startPosition, SEEK_SET);

  fwrite(&directory.size, sizeof(size_t), 1, archiverFile);
  fwrite(&directory.numMembers, sizeof(int), 1, archiverFile);
  // Write members info
  for (int i = 0; i < directory.numMembers; i++) {
    char *pathName = directory.membersInfo[i].pathName;
    fwrite(&directory.membersInfo[i].index, sizeof(int), 1, archiverFile);
    fwrite(&directory.membersInfo[i].pathNameLen, sizeof(int), 1, archiverFile);
    fwrite(pathName, sizeof(char), strlen(directory.membersInfo[i].pathName),
           archiverFile);
    fwrite(&directory.membersInfo[i].startPosition, sizeof(unsigned long), 1,
           archiverFile);
    fwrite(&directory.membersInfo[i].userID, sizeof(unsigned int), 1,
           archiverFile);
    fwrite(&directory.membersInfo[i].permissions, sizeof(mode_t), 1,
           archiverFile);
    fwrite(&directory.membersInfo[i].size, sizeof(size_t), 1, archiverFile);
    fwrite(&directory.membersInfo[i].updatedAt, sizeof(time_t), 1,
           archiverFile);
  }

  fclose(archiverFile);
}
