#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include <sys/types.h>

#include "index.h"
#include "invertedIndex.h"
#include "generalInfo.h"

typedef struct worker{
	pid_t pid;
	mapIndex* index;
	rootNode* root;
	generalInfo* info;
}worker;

#endif