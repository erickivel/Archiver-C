#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void createDirectoryHierarchy(char *directories) {
  char *lastDir = strrchr(directories, '/');

  if (!lastDir) {
    return;
  }

  char *copyStr = strdup(directories);
  copyStr[strlen(directories) - strlen(lastDir)] = '\0';

  createDirectoryHierarchy(copyStr);

  mkdir(directories, 0700);

  free(copyStr);
}

void archiverExtract(struct Archiver *archiver, struct FilePaths *filePaths) {
  for (int i = 0; i < filePaths->size; i++) {
    int memberIndex;
    struct MemberInfo *memberInfo =
        findMemberInfo(archiver, filePaths->names[i], &memberIndex);

    if (!memberInfo) {
      printf("The file '%s' doesn't exist on the archiver '%s'\n",
             filePaths->names[i], archiver->pathName);
      return;
    }

    // Create directory hierarchy
    char *hasDirectory = strrchr(memberInfo->pathName, '/');

    char *directories;
    if (hasDirectory) {
      if (memberInfo->pathName[0] == '/') {
        directories = malloc(sizeof(char) * (memberInfo->pathNameLen + 2));
        directories[0] = '.';
        strcpy(directories + 1, memberInfo->pathName);
      } else if (memberInfo->pathName[0] != '.' &&
                 memberInfo->pathName[1] != '/') {
        directories = malloc(sizeof(char) * (memberInfo->pathNameLen + 3));
        directories[0] = '.';
        directories[1] = '/';
        strcpy(directories + 2, memberInfo->pathName);
      } else {
        directories = malloc(sizeof(char) * (memberInfo->pathNameLen + 1));
        strcpy(directories, memberInfo->pathName);
      }
      directories[strlen(directories) - strlen(hasDirectory)] = '\0';
      createDirectoryHierarchy(directories);
    }

    char *pathNameWithFile =
        malloc(sizeof(char) * (strlen(directories) + strlen(hasDirectory) + 1));
    strcpy(pathNameWithFile, directories);
    strcat(pathNameWithFile, hasDirectory);

    // create file
    FILE *archiverFile = fopen(archiver->pathName, "rb");
    FILE *extractedFile = fopen(pathNameWithFile, "w+b");

    char buffer[BUFFER_SIZE];
    int iterations = memberInfo->size / BUFFER_SIZE;
    int bytesLeft = memberInfo->size % BUFFER_SIZE;

    fseek(archiverFile, memberInfo->startPosition, SEEK_SET);

    // copy content into it
    for (int i = 0; i < iterations; i++) {
      fread(buffer, sizeof(char), BUFFER_SIZE, archiverFile);
      fwrite(buffer, sizeof(char), BUFFER_SIZE, extractedFile);
    }

    fread(buffer, sizeof(char), bytesLeft, archiverFile);
    fwrite(buffer, sizeof(char), bytesLeft, extractedFile);

    free(pathNameWithFile);
    free(directories);
    fclose(extractedFile);
    fclose(archiverFile);
  }
}
