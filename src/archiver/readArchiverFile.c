#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Archiver *readArchiverFile(char *archiverPath) {
  FILE *archiverFile = fopen(archiverPath, "r");

  struct Archiver *archiver = malloc(sizeof(struct Archiver));
  archiver->pathName = malloc(sizeof(char) * (strlen(archiverPath) + 1));
  strcpy(archiver->pathName, archiverPath);

  if (!archiverFile) {
    archiverFile = fopen(archiverPath, "w+");
    archiver->directory.numMembers = 0;
    archiver->directory.startPosition = 0;
    archiver->directory.size = sizeof(int) + sizeof(size_t);
    archiver->directory.membersInfo = malloc(sizeof(struct MemberInfo));

    fclose(archiverFile);
    return archiver;
  }

  fread(&archiver->directory.startPosition, sizeof(size_t), 1, archiverFile);

  fseek(archiverFile, archiver->directory.startPosition, SEEK_SET);

  fread(&archiver->directory.size, sizeof(size_t), 1, archiverFile);
  int numMembers;
  fread(&numMembers, sizeof(int), 1, archiverFile);

  archiver->directory.numMembers = numMembers;

  archiver->directory.membersInfo =
      malloc(sizeof(struct MemberInfo) * numMembers);

  for (int i = 0; i < archiver->directory.numMembers; i++) {
    fread(&archiver->directory.membersInfo[i].index, sizeof(int), 1,
          archiverFile);
    fread(&archiver->directory.membersInfo[i].pathNameLen, sizeof(int), 1,
          archiverFile);
    int pathNameLen = archiver->directory.membersInfo[i].pathNameLen;
    archiver->directory.membersInfo[i].pathName =
        malloc(sizeof(char) * (pathNameLen + 1));
    fread(archiver->directory.membersInfo[i].pathName, sizeof(char),
          pathNameLen, archiverFile);
    archiver->directory.membersInfo[i].pathName[pathNameLen] = '\0';
    fread(&archiver->directory.membersInfo[i].startPosition,
          sizeof(unsigned long), 1, archiverFile);
    fread(&archiver->directory.membersInfo[i].userID, sizeof(unsigned int), 1,
          archiverFile);
    fread(&archiver->directory.membersInfo[i].permissions, sizeof(mode_t), 1,
          archiverFile);
    fread(&archiver->directory.membersInfo[i].size, sizeof(size_t), 1,
          archiverFile);
    fread(&archiver->directory.membersInfo[i].updatedAt, sizeof(time_t), 1,
          archiverFile);
  }

  fclose(archiverFile);

  return archiver;
}
