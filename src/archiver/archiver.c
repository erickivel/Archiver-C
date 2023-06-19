#include "../archiver.h"

void archiverMove(struct Archiver *archiver, char *targetFilePath,
                  struct FilePaths *filePaths);

void archiverExtract(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverRemove(struct Archiver *archiver, struct FilePaths *filePaths);

void archiverList(struct Archiver *archiver);

void archiverHelp();
