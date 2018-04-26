#ifndef _WORKER_H_
#define _WORKER_H_

#include <sys/types.h>

#include "index.h"
//#include "invertedIndex.h"


int returnNumPaths(int *w,int noPaths);
mapIndex** populateIndexes(char** fileNames,char* path,int noFiles,int previousNoFiles,mapIndex** indexes);
void destroyIndexes(int length,mapIndex** indexes);

#endif