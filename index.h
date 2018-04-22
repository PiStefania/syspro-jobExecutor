#ifndef _INDEX_H_
#define _INDEX_H_

#include "generalInfo.h"


typedef struct mapIndex{
	FILE* fp;
	char** documents;			//document
	int words;				//words of each document
}mapIndex;



mapIndex* populateIndex(int lines,FILE *fp,generalInfo* info);
void printMapIndex(mapIndex* index,int noElems);
void destroyMapIndex(mapIndex* index,int noElems);

#endif
