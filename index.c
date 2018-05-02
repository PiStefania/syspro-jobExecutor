#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

//populate an index
mapIndex* populateIndex(int lines,FILE *fp,char* fileName){
	mapIndex* index = (mapIndex*) malloc(sizeof(mapIndex));
	index->documents = (char**)malloc(lines*sizeof(char*));
	index->characters = 0;
	index->noDocs = 0;
	index->words = 0;
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
			index->documents[counter] = NULL;
		}
		else{
			index->documents[counter] = malloc((strlen(tempLine)+1)*sizeof(char));
			strcpy(index->documents[counter],tempLine);
			index->characters += strlen(tempLine);
		}
		counter++;
	}
	index->noDocs = counter;
	index->fileName = malloc((strlen(fileName)+1)*sizeof(char));
	strcpy(index->fileName,fileName);
	if(line){
		free(line);
		line = NULL;
	}
	return index;
}

//print index
void printMapIndex(mapIndex* index,int noElems){
	printf("Printing Map Index\n");
	for(int i=0; i < noElems; i++){
		printf("Document: '%s'\n",index->documents[i]);
	}
}

//destroy index
void destroyMapIndex(mapIndex* index,int noElems){
	for(int i=0;i<noElems;i++){
		if(index->documents[i]!=NULL){
			free(index->documents[i]);
			index->documents[i] = NULL;
		}
	}
	free(index->documents);
	index->documents = NULL;
	free(index->fileName);
	index->fileName = NULL;
	free(index);
	index = NULL;
}