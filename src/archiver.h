#include <sys/types.h>
#include <time.h>

struct MemberInfo {
  int index;
  int pathNameLen;
  char *pathName;
  unsigned long startPosition;
  unsigned int userID;
  mode_t permissions;
  size_t size;
  time_t updatedAt;
};

struct Directory {
  size_t startPosition;
  size_t size;
  int numMembers;
  struct MemberInfo *membersInfo;
};

struct Archiver {
  char *pathName;
  struct Directory directory;
};

struct FilePaths {
  char **names;
  int size;
};

struct Archiver *readArchiverFile(char *archiverPath);

void writeDirectoryOnFile(struct Archiver *archiver, char *filePath);

struct MemberInfo *findMemberInfo(struct Archiver *archiver, char *filePath);

void removeMember(struct Archiver *archiver, char *memberPath);

void archiverInsert(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverUpdate(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverMove(struct Archiver *archiver, char *targetFilePath,
                  struct FilePaths *filePaths);

void archiverExtract(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverRemove(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverList(struct Archiver *archiver);

void archiverHelp();
