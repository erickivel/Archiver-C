#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define TEMP_ARCHIVE_NAME "temp.vpp"
#define BUFFER_SIZE 1024
// #define st_mtime mtim.tv_sec
// #define st_mtim fileInfo.mtim.tv_sec

struct Archiver *readArchiverFile(char *archiverPath) {
  FILE *archiverFile = fopen(archiverPath, "r");

  struct Archiver *archiver = malloc(sizeof(struct Archiver));
  archiver->pathName = malloc(sizeof(char) * (strlen(archiverPath) + 1));
  strcpy(archiver->pathName, archiverPath);

  if (!archiverFile) {
    archiverFile = fopen(archiverPath, "w+");
    archiver->directory.numMembers = 0;
    archiver->directory.size = sizeof(int) + sizeof(size_t);
    archiver->directory.membersInfo = malloc(sizeof(struct MemberInfo));

    fclose(archiverFile);
    return archiver;
  }

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

struct MemberInfo *searchMemberInfo(struct Archiver *archiver, char *filePath) {
  for (int i = 0; i < archiver->directory.numMembers; i++) {
    if (!strcmp(archiver->directory.membersInfo[i].pathName, filePath)) {
      return &archiver->directory.membersInfo[i];
    }
  }

  return NULL;
}

struct MemberInfo *createMemberInfo(char *filePath) {
  struct MemberInfo *memberInfo = malloc(sizeof(struct MemberInfo));
  struct stat fileInfo;

  stat(filePath, &fileInfo);

  memberInfo->index = 0;
  memberInfo->pathNameLen = strlen(filePath);
  memberInfo->pathName = malloc(sizeof(char) * (strlen(filePath) + 1));
  strcpy(memberInfo->pathName, filePath);
  memberInfo->startPosition = 0;
  memberInfo->userID = fileInfo.st_uid;
  memberInfo->permissions = fileInfo.st_mode;
  memberInfo->size = fileInfo.st_size;
  memberInfo->updatedAt = fileInfo.st_mtime;

  return memberInfo;
}

void writeDirectoryOnFile(struct Archiver *archiver, char *filePath) {
  FILE *archiverFile = fopen(filePath, "w");
  struct Directory directory = archiver->directory;

  // Write directory size
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

void updateDirectorySize(struct Directory *directory) {
  directory->size = sizeof(size_t) + sizeof(int) +
                    directory->numMembers * sizeof(struct MemberInfo);
}

void archiverInsert(struct Archiver *archiver, struct FilePaths *filePaths) {
  if (filePaths->size > 0) {
    FILE *archiverFile = fopen(archiver->pathName, "r");
    FILE *tempArchiverFile = fopen(TEMP_ARCHIVE_NAME, "w");
    for (int i = 0; i < filePaths->size; i++) {
      struct MemberInfo *memberInfo =
          searchMemberInfo(archiver, filePaths->names[i]);

      FILE *fileToInsert = fopen(filePaths->names[i], "r");

      if (!archiverFile || !tempArchiverFile || !fileToInsert) {
        perror("It was not possible to open the archiver files!");
        exit(1);
      }

      if (memberInfo) {
        return;
        // Update file
        // Update archiver directory
        archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
            *memberInfo;
        archiver->directory.numMembers++;

      } else {
        // Insert file
        // Update archiver directory
        memberInfo = createMemberInfo(filePaths->names[i]);
        archiver->directory.numMembers++;
        archiver->directory.membersInfo =
            realloc(archiver->directory.membersInfo,
                    sizeof(struct MemberInfo) * archiver->directory.numMembers);

        updateDirectorySize(&archiver->directory);

        fseek(tempArchiverFile, 0, SEEK_END);
        if (archiver->directory.numMembers == 1) {
          memberInfo->startPosition = archiver->directory.size + 1;
        } else {
          // The new member info isn't on the temp archive yet
          memberInfo->startPosition =
              ftell(tempArchiverFile) + sizeof(struct MemberInfo);
        }

        archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
            *memberInfo;

        writeDirectoryOnFile(archiver, TEMP_ARCHIVE_NAME);
        // Insert file content
        char buffer[BUFFER_SIZE];
        int iterations = memberInfo->size / BUFFER_SIZE;
        int bytesLeft = memberInfo->size % BUFFER_SIZE;

        for (int i = 0; i < iterations; i++) {
          fread(&buffer, sizeof(char), BUFFER_SIZE, fileToInsert);
          fwrite(&buffer, sizeof(char), BUFFER_SIZE, tempArchiverFile);
        }

        fread(&buffer, sizeof(char), bytesLeft, fileToInsert);
        fwrite(&buffer, sizeof(char), bytesLeft, tempArchiverFile);
      }

      fclose(fileToInsert);
    }
    fclose(archiverFile);
    fclose(tempArchiverFile);
  }
}

void archiverUpdate(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverMove(struct Archiver *archiver, char *targetFilePath,
                  struct FilePaths *filePaths);

void archiverExtract(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverRemove(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverList(struct Archiver *archiver);

void archiverHelp();
