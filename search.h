#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "invertedIndex.h"
#include "index.h"

typedef struct searchInfo{
	searchNode* nodes;
	int length;
}searchInfo

typedef struct searchNode{
	char* fileName;
	int* lines;
}searchInfo;

char* searchFiles(rootNode* root,char** searchWords,int words);
searchInfo* createSearchNodesWord(searchInfo* nodes,rootNode* root,char* word,char** fileNames);
int searchFileName(searchInfo* nodes,char* fileName);

#endif