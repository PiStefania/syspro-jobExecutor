#ifndef _WC_H_
#define _WC_H_

#include "worker.h"

typedef struct wcInfo{
	int characters;
	int lines;
	int words;
}wcInfo;


wcInfo* returnInfoStruct(indexesArray* indexesArr);

#endif