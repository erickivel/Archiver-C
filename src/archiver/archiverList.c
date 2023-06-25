#include "../archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int compareIndex(const void *a, const void *b) {
  struct MemberInfo *memberA = (struct MemberInfo *)a;
  struct MemberInfo *memberB = (struct MemberInfo *)b;

  return memberA->index - memberB->index;
}

void printPermissions(int permissions) {
  printf((permissions & S_IRUSR) ? "r" : "-");
  printf((permissions & S_IWUSR) ? "w" : "-");
  printf((permissions & S_IXUSR) ? "x" : "-");

  printf((permissions & S_IRGRP) ? "r" : "-");
  printf((permissions & S_IWGRP) ? "w" : "-");
  printf((permissions & S_IXGRP) ? "x" : "-");

  printf((permissions & S_IROTH) ? "r" : "-");
  printf((permissions & S_IWOTH) ? "w" : "-");
  printf((permissions & S_IXOTH) ? "x " : "- ");
}

void printDate(time_t date) {
  struct tm *timeInfo;
  char formattedDate[80];

  timeInfo = localtime(&date);

  strftime(formattedDate, sizeof(formattedDate), "%b %d %H:%M %Y", timeInfo);

  printf("%s ", formattedDate);
}

void archiverList(struct Archiver *archiver) {
  struct MemberInfo *membersInfoCopy =
      malloc(sizeof(struct MemberInfo) * archiver->directory.numMembers);

  // Copy members info
  for (int i = 0; i < archiver->directory.numMembers; i++) {
    membersInfoCopy[i] = archiver->directory.membersInfo[i];
  }

  // Order members info
  qsort(membersInfoCopy, archiver->directory.numMembers,
        sizeof(struct MemberInfo), compareIndex);

  for (int i = 0; i < archiver->directory.numMembers; i++) {
    printf("%d: ", membersInfoCopy[i].index);
    printPermissions(membersInfoCopy[i].permissions);
    printf("%d ", membersInfoCopy[i].userID);
    printf("%8lu ", membersInfoCopy[i].size);
    printDate(membersInfoCopy[i].updatedAt);
    printf("%s \n", membersInfoCopy[i].pathName);
  }

  free(membersInfoCopy);
}
