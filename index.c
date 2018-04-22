#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

//checks if file is formatted correctly
/*int checkFileGetLines(FILE *fp){
	int read;
	size_t len = 0;
	int lines = 0;
	
	//change after
	int counterIds = 0;
	int id;
	char* line = NULL;
	while((read = getline(&line, &len, fp)) != -1){
		//check line whitespace
		char* temp = strtok(line,"\n");
		if(temp == NULL){
			continue;
		}
		temp = strtok(temp," \t");
		if(temp == NULL){
			continue;
		}
		
		//check ids
		id = atoi(temp);
		if(id == counterIds){
			lines++;
			counterIds++;
		}else{
			free(temp);
			temp = NULL;
			return -1;
		}
		
		
	}
	if(line){
		free(line);
		line = NULL;
	}
	
	return lines;
}*/

mapIndex* populateIndex(int lines,FILE *fp,generalInfo* info){
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
		
		//get document
		tempLine = strtok(tempLine," \t");
		char* document = strtok(NULL,"");
		char* temp;
		if(document != NULL){
			temp = malloc((strlen(document)+1)*sizeof(char));
			strcpy(temp,document);
		}
		if(strtok(document," \t") == NULL){
			free(temp);
			temp = NULL;
		}
		
		//insert to index
		if(temp == NULL){
			index[counter].document = NULL;
		}
		else{
			index[counter].document = malloc((strlen(temp)+1)*sizeof(char));
			strcpy(index[counter].document,temp);
			
			free(temp);
			temp = NULL;
		}
		counter++;
	}
	
	//set total docs
	info->totalDocuments = counter;
	
	if(line){
		free(line);
		line = NULL;
	}

	return index;
}

void printMapIndex(mapIndex* index,int noElems){
	printf("Printing Map Index\n");
	for(int i=0; i < noElems; i++){
		printf("ID: '%d', document: '%s'\n",i,index[i].document);
	}
}

void destroyMapIndex(mapIndex* index,int noElems){
	for(int i=0;i<noElems;i++){
		if(index[i].document!=NULL){
			free(index[i].document);
			index[i].document = NULL;
		}
	}
	
	free(index);
	index = NULL;
	
	
}