#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

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

void updateDirectorySize(struct Directory *directory) {
  directory->size = sizeof(size_t) + sizeof(int) +
                    directory->numMembers * sizeof(struct MemberInfo);
}

void archiverInsert(struct Archiver *archiver, struct FilePaths *filePaths) {
  if (filePaths->size > 0) {
    FILE *archiverRead = fopen(archiver->pathName, "r");
    FILE *archiverWrite = fopen(archiver->pathName, "w");

    for (int i = 0; i < filePaths->size; i++) {
      struct MemberInfo *memberInfo =
          findMemberInfo(archiver, filePaths->names[i]);

      FILE *fileToInsert = fopen(filePaths->names[i], "r");

      if (!archiverRead || !archiverWrite || !fileToInsert) {
        perror("It was not possible to open the archiver files!");
        exit(1);
      }

      if (memberInfo) {
        return;
        // Update file & archiver directory
        archiver->directory.membersInfo[archiver->directory.numMembers - 1] =
            *memberInfo;
        archiver->directory.numMembers++;

      } else {
        // Insert file & update archiver directory
        memberInfo = createMemberInfo(filePaths->names[i]);

        appendMemberContent(archiver, &memberInfo);

        // Update directory and member info
        archiver->directory.numMembers++;
        archiver->directory.membersInfo =
            realloc(archiver->directory.membersInfo,
                    sizeof(struct MemberInfo) * archiver->directory.numMembers);
        archiver->directory.size =
            sizeof(size_t) + sizeof(int) +
            archiver->directory.numMembers * sizeof(struct MemberInfo);
        memberInfo->index = archiver->directory.numMembers;
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
    fclose(archiverRead);
    fclose(archiverWrite);
  }
}
