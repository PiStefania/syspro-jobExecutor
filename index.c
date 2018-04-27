#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

mapIndex* populateIndex(int lines,FILE *fp,char* fileName){
	mapIndex* index = (mapIndex*) malloc(lines*sizeof(mapIndex));
	int read;
	char *line = NULL;
	size_t len = 0;
	int counter = 0;
	while ((read = getline(&line, &len, fp)) != -1)
	{
		char* tempLine = strtok(line,"\n");
		if(tempLine == NULL){
			continue;
		}
		
		//insert to index
		if(tempLine == NULL){
			index[counter].document = NULL;
		}
		else{
			index[counter].document = malloc((strlen(tempLine)+1)*sizeof(char));
			strcpy(index[counter].document,tempLine);
		}
		counter++;
	}
	index->words = 0;
	index->noDocs = counter;
	index->fileName = malloc((strlen(fileName)+1)*sizeof(char));
	strcpy(index->fileName,fileName);
	if(line){
		free(line);
		line = NULL;
	}
	return index;
}

void printMapIndex(mapIndex* index,int noElems){
	printf("Printing Map Index\n");
	for(int i=0; i < noElems; i++){
		printf("Document: '%s'\n",index[i].document);
	}
}

void destroyMapIndex(mapIndex* index,int noElems){
	for(int i=0;i<noElems;i++){
		if(index[i].document!=NULL){
			free(index[i].document);
			index[i].document = NULL;
		}
	}
	free(index->fileName);
	index->fileName = NULL;
	free(index);
	index = NULL;
}