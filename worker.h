#ifndef _WORKER_H_
#define _WORKER_H_

#include <sys/types.h>

#include "index.h"
#include "invertedIndex.h"
#include "exit.h"

int executeOptions;

typedef struct indexesArray{
	mapIndex** indexes;
	int length;
}indexesArray;

int returnNumPaths(int *w,int noPaths);
indexesArray* populateIndexes(char** fileNames,char* path,int noFiles,indexesArray* indexesArr);
void populateTrieWorker(rootNode *root,indexesArray* indexesArr);
void destroyIndexes(indexesArray* indexesArr);

//FIFOS
void createFIFOS(int w);
void parentFIFOS(int w,char* line);
int childFIFOS(int worker,indexesArray* indexesArr,rootNode* root,int logfd);
void deleteFIFOS(int w);

#endif