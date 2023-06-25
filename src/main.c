#include "archiver.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct FilePaths *readArgs(int argc, char *argv[], int startIndex) {
  struct FilePaths *filePaths = malloc(sizeof(struct FilePaths));
  int idx = 0;
  filePaths->size = argc - startIndex;
  filePaths->names = malloc(sizeof(char *) * filePaths->size);
  for (int i = startIndex; i < argc; i++) {
    filePaths->names[idx] = malloc(sizeof(char) * (strlen(argv[i]) + 1));
    strcpy(filePaths->names[idx], argv[i]);
    idx++;
  }

  return filePaths;
}

int main(int argc, char *argv[]) {

  int opt;
  struct FilePaths *filePaths;

  struct Archiver *archiver;

  while ((opt = getopt(argc, argv, "iam:xrch")) != -1) {
    switch (opt) {
    case 'i':
      archiver = readArchiverFile(argv[2]);
      filePaths = readArgs(argc, argv, 3);
      archiverInsert(archiver, filePaths);
      break;
    case 'a':
      archiver = readArchiverFile(argv[2]);
      filePaths = readArgs(argc, argv, 3);
      archiverUpdate(archiver, filePaths);
      break;
    case 'm':
      archiver = readArchiverFile(argv[3]);
      filePaths = readArgs(argc, argv, 4);
      archiverMove(archiver, optarg, filePaths);
      break;
    case 'x':
      archiver = readArchiverFile(argv[2]);
      filePaths = readArgs(argc, argv, 3);
      archiverExtract(archiver, filePaths);
      break;
    case 'r':
      archiver = readArchiverFile(argv[2]);
      filePaths = readArgs(argc, argv, 3);
      archiverRemove(archiver, filePaths);
      break;
    case 'c':
      archiver = readArchiverFile(argv[2]);
      filePaths = NULL;
      archiverList(archiver);
      break;
    case 'h':
      printf("Help\n");
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  freeArchiver(archiver);

  if (filePaths) {
    for (int i = 0; i < filePaths->size; i++) {
      free(filePaths->names[i]);
    }
    free(filePaths->names);
    free(filePaths);
  }

  return 0;
}
