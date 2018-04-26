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

mapIndex** populateIndexes(char** fileNames,char* path,int noFiles,int previousNoFiles,mapIndex** indexes){
	if(indexes == NULL){
		indexes = malloc(noFiles*sizeof(mapIndex*));
	}else{
		indexes = realloc(indexes,(noFiles + previousNoFiles)*sizeof(mapIndex*)); 
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
			indexes[i] = populateIndex(lines,fp,fullName);
		}
		fclose(fp);
		free(fullName);
	}
	return indexes;
}




void destroyIndexes(int length,mapIndex** indexes){
	for(int i=0;i<length;i++){
		printMapIndex(indexes[i],indexes[i]->noDocs);
		destroyMapIndex(indexes[i],indexes[i]->noDocs);
	}
	free(indexes);
	indexes = NULL;
}