#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {

  // struct stat st;
  // stat("text.txt", &st);
  // size_t s = st.st_size;
  // printf("%lu", s);

  struct Archiver *archiver1 = readArchiverFile("backup.vpp");

  struct FilePaths filePaths;
  char *name1 = "text.txt";
  char *name2 = "new.txt";

  filePaths.size = 2;
  filePaths.names = malloc(sizeof(char) * filePaths.size);
  filePaths.names[0] = name1;
  filePaths.names[1] = name2;

  archiverInsert(archiver1, &filePaths);
  struct Archiver *archiver = readArchiverFile("backup.vpp");
  printf("\n Directory num member %d\n", archiver->directory.numMembers);
  printf("\n Directory start position %lu\n",
         archiver->directory.startPosition);
  for (int i = 0; i < archiver->directory.numMembers; i++) {
    printf("\n File pathlen %d\n",
           archiver->directory.membersInfo[i].pathNameLen);
    printf("\n File pathName %s\n",
           archiver->directory.membersInfo[i].pathName);
    printf("\n File updatedAt  %lu\n ",
           archiver->directory.membersInfo[i].updatedAt);
    printf("\n File permissions %d\n",
           archiver->directory.membersInfo[i].permissions);
    printf("\n File index %d\n", archiver->directory.membersInfo[i].index);
    printf("\n File start position %lu\n",
           archiver->directory.membersInfo[i].startPosition);
    printf("\n File userId %d\n", archiver->directory.membersInfo[i].userID);
    printf("\n File size %lu\n", archiver->directory.membersInfo[i].size);
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