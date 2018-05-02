#include <stdio.h>
#include <stdlib.h>
#include "wc.h"

//return info of wc of a worker
wcInfo* returnInfoStruct(indexesArray* indexesArr){
	wcInfo* info = malloc(sizeof(wcInfo));
	info->lines = 0;
	info->words = 0;
	info->characters = 0;
	for(int i=0;i<indexesArr->length;i++){
		info->lines += indexesArr->indexes[i]->noDocs;
		info->words += indexesArr->indexes[i]->words;
		info->characters += indexesArr->indexes[i]->characters;
	}
	return info;
}
