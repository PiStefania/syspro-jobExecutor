#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generalInfo.h"
#define K1 1.2
#define B 0.75

void initializeGeneralInfo(generalInfo** info){
	*info = malloc(sizeof(generalInfo));
	(*info)->totalWords = 0;
	(*info)->totalDocuments = 0;
	(*info)->k1 = K1;
	(*info)->b = B;
}

void destroyGeneralInfo(generalInfo** info){
	free(*info);
	*info = NULL;
}
