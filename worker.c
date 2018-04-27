#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "worker.h"
#include "variousMethods.h"

int returnNumPaths(int *w,int noPaths){
	int numPaths = 0;
	if(*w == noPaths){
		numPaths = 1;
	}else{
		if(noPaths % *w != 0){
			*w = noPaths;
			numPaths = 1;
		}else{
			numPaths = noPaths / *w;
		}
	}
	return numPaths;
}

indexesArray* populateIndexes(char** fileNames,char* path,int noFiles,indexesArray* indexesArr){
	if(indexesArr->indexes == NULL){
		indexesArr->indexes = malloc(noFiles*sizeof(mapIndex*));
	}else{
		indexesArr->indexes = realloc(indexesArr->indexes,(indexesArr->length+noFiles)*sizeof(mapIndex*)); 
	}
	for(int i=0;i<noFiles;i++){
		//create and populate each index with each file
		char* fullName = malloc((strlen(fileNames[i])+strlen(path)+2)*sizeof(char));
		strcpy(fullName,path);
		strcat(fullName,fileNames[i]);
		FILE *fp = fopen(fullName, "r");
		if(fp == NULL){
			printf("Error with opening '%s'\n",fileNames[i]);
			continue;
		}else{
			int lines = countFileLines(fp);
			rewind(fp);
			int newPos = i + indexesArr->length;
			indexesArr->indexes[newPos] = populateIndex(lines,fp,fullName);
		}
		fclose(fp);
		free(fullName);
	}
	indexesArr->length += noFiles;
	return indexesArr;
}

void populateTrieWorker(rootNode *root,indexesArray* indexesArr){
	for(int i=0;i<indexesArr->length;i++){
		populateTrie(root,indexesArr->indexes[i],indexesArr->indexes[i]->noDocs);
	}
}

void destroyIndexes(indexesArray* indexesArr){
	for(int i=0;i<indexesArr->length;i++){
		destroyMapIndex(indexesArr->indexes[i],indexesArr->indexes[i]->noDocs);
	}
	free(indexesArr->indexes);
	indexesArr->indexes = NULL;
	free(indexesArr);
	indexesArr = NULL;
}