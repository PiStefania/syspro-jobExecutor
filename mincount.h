#ifndef _MINCOUNT_H_
#define _MINCOUNT_H_

#include "invertedIndex.h"

typedef struct minCountInfo{
	char* fileName;
	int timesAppeared;
}minCountInfo;

minCountInfo* returnMinCount(rootNode* root,char* word);
minCountInfo* searchPostListMin(postingListsHead* list);
void destroyMinCountInfo(minCountInfo* info);

#endif