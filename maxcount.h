#ifndef _MAXCOUNT_H_
#define _MAXCOUNT_H_

#include "invertedIndex.h"

typedef struct maxCountInfo{
	char* fileName;
	int timesAppeared;
}maxCountInfo;

maxCountInfo* returnMaxCount(rootNode* root,char* word);
maxCountInfo* searchPostListMax(postingListsHead* list);
void destroyMaxCountInfo(maxCountInfo* info);
int maxCount(postingListsHead* list);

#endif