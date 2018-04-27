#ifndef _WORKER_H_
#define _WORKER_H_

#include <sys/types.h>

#include "index.h"
#include "invertedIndex.h"

typedef struct indexesArray{
	mapIndex** indexes;
	int length;
}indexesArray;

int returnNumPaths(int *w,int noPaths);
indexesArray* populateIndexes(char** fileNames,char* path,int noFiles,indexesArray* indexesArr);
void populateTrieWorker(rootNode *root,indexesArray* indexesArr);
void destroyIndexes(indexesArray* indexesArr);

#endif