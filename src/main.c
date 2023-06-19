#include "archiver.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct FilePaths *readArgs(int argc, char *argv[], int startIndex) {
  struct FilePaths *filePaths = malloc(sizeof(struct FilePaths));
  int idx = 0;
  filePaths->size = argc - 3;
  filePaths->names = malloc(sizeof(char) * filePaths->size);
  for (int i = startIndex; i < argc; i++) {
    filePaths->names[idx] = malloc(sizeof(char) * strlen(argv[i]) + 1);
    strcpy(filePaths->names[idx], argv[i]);
    idx++;
  }

  return filePaths;
}

int main(int argc, char *argv[]) {

  int opt;
  struct FilePaths *filePaths;

  struct Archiver *archiver;

  while ((opt = getopt(argc, argv, "i:a:m:x:r:c:h")) != -1) {
    switch (opt) {
    case 'i':
      archiver = readArchiverFile(optarg);
      filePaths = readArgs(argc, argv, 3);
      archiverInsert(archiver, filePaths);
      break;
    case 'a':
      archiver = readArchiverFile(optarg);
      filePaths = readArgs(argc, argv, 3);
      archiverUpdate(archiver, filePaths);
      break;
    case 'm':
      break;
    case 'x':
      break;
    case 'r':
      break;
    case 'c':
      break;
    case 'h':
      printf("Help\n");
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  printf("Directory num member %d\n", archiver->directory.numMembers);
  printf("Directory  size %lu\n", archiver->directory.size);
  printf("Directory start position %lu\n", archiver->directory.startPosition);
  printf("======================================\n");
  for (int i = 0; i < archiver->directory.numMembers; i++) {
    printf("File pathlen %d\n", archiver->directory.membersInfo[i].pathNameLen);
    printf("File pathName %s\n ", archiver->directory.membersInfo[i].pathName);
    printf(" File updatedAt %lu\n ",
           archiver->directory.membersInfo[i].updatedAt);
    printf("File permissions %d\n",
           archiver->directory.membersInfo[i].permissions);
    printf("File index %d\n", archiver->directory.membersInfo[i].index);
    printf("File start position %lu\n",
           archiver->directory.membersInfo[i].startPosition);
    printf("File userId %d\n", archiver->directory.membersInfo[i].userID);
    printf("File size %lu\n", archiver->directory.membersInfo[i].size);
    printf("======================================\n");
  }

  struct MemberInfo firstMember = archiver->directory.membersInfo[0];

  char firstContent[firstMember.size];

  FILE *contFile = fopen(archiver->pathName, "rb");

  fseek(contFile, firstMember.startPosition, SEEK_SET);

  fread(firstContent, sizeof(char), firstMember.size, contFile);

  firstContent[firstMember.size] = '\0';

  printf("Content: %s", firstContent);

  return 0;
}
