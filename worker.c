#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/errno.h>
#include "worker.h"
#include "variousMethods.h"
#include "namedPipes.h"

#define PERMS 0777


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

void createFIFOS(int w){
	for(int i=0;i<w;i++){
		int numberLength;
		if(i == 0){
			numberLength = 1;
		}else{
			numberLength = floor(log10(abs(i))) + 1;
		}
		char* FIFO1 = malloc((strlen("./tmp/fifo1worker")+ 4 + numberLength)*sizeof(char));
		char* FIFO2 = malloc((strlen("./tmp/fifo2worker")+ 4 + numberLength)*sizeof(char));
		sprintf(FIFO1,"./tmp/fifo1worker%d",i);
		sprintf(FIFO2,"./tmp/fifo2worker%d",i);
			
		//create fifos for read and writing
		if((mkfifo(FIFO1, PERMS) < 0) && (errno != EEXIST) ) {
			perror("can't create fifo");
		}
		if((mkfifo(FIFO2, PERMS) < 0) && (errno != EEXIST)) {
			unlink(FIFO1);
			perror("can't create fifo");
		}
		free(FIFO1);
		FIFO1 = NULL;
		free(FIFO2);
		FIFO2 = NULL;
	}
}
void parentFIFOS(int w,char* line){
	for(int i=0;i<w;i++){
		int numberLength;
		if(i == 0){
			numberLength = 1;
		}else{
			numberLength = floor(log10(abs(i))) + 1;
		}
		char* FIFO1 = malloc((strlen("./tmp/fifo1worker")+ 4 + numberLength)*sizeof(char));
		char* FIFO2 = malloc((strlen("./tmp/fifo2worker")+ 4 + numberLength)*sizeof(char));
		sprintf(FIFO1,"./tmp/fifo1worker%d",i);
		sprintf(FIFO2,"./tmp/fifo2worker%d",i);
		
		int readfd, writefd;
		
		if((readfd = open(FIFO1, O_RDONLY)) < 0) {
			perror("server: can't open read fifo");
		}
		if((writefd = open(FIFO2, O_WRONLY)) < 0) {
			perror("server: can't open write fifo");
		}
		
		serverSide(readfd, writefd,line);
		close(readfd);
		close(writefd);
		
		free(FIFO1);
		FIFO1 = NULL;
		free(FIFO2);
		FIFO2 = NULL;
	}
}

int childFIFOS(int worker){	

	int numberLength;
	if(worker == 0){
		numberLength = 1;
	}else{
		numberLength = floor(log10(abs(worker))) + 1;
	}
	
	char* FIFO1 = malloc((strlen("./tmp/fifo1worker")+ 4 + numberLength)*sizeof(char));
	char* FIFO2 = malloc((strlen("./tmp/fifo2worker")+ 4 + numberLength)*sizeof(char));
	sprintf(FIFO1,"./tmp/fifo1worker%d",worker);
	sprintf(FIFO2,"./tmp/fifo2worker%d",worker);
	
	int readfd, writefd;
	/* Open the FIFOs. We assume server has
	already created them. */
	if((writefd = open(FIFO1, O_WRONLY)) < 0)
	{
		perror("client: can't open write fifo \n");
	}
	if((readfd = open(FIFO2, O_RDONLY)) < 0)
	{
		perror("client: can't open read fifo \n");
	}
	
	int ret = clientSide(readfd, writefd);
	close(readfd);
	close(writefd);
	
	free(FIFO1);
	FIFO1 = NULL;
	free(FIFO2);
	FIFO2 = NULL;
	
	return ret;
}

void deleteFIFOS(int w){
	for(int i=0;i<w;i++){
		int numberLength;
		if(i == 0){
			numberLength = 1;
		}else{
			numberLength = floor(log10(abs(i))) + 1;
		}
		char* FIFO1 = malloc((strlen("./tmp/fifo1worker")+ 4 + numberLength)*sizeof(char));
		char* FIFO2 = malloc((strlen("./tmp/fifo2worker")+ 4 + numberLength)*sizeof(char));
		sprintf(FIFO1,"./tmp/fifo1worker%d",i);
		sprintf(FIFO2,"./tmp/fifo2worker%d",i);
		
		//delete FIFOS
		if(unlink(FIFO1) < 0) {
			perror("client: can't unlink \n");
		}
		if(unlink(FIFO2) < 0) {
			perror("client: can't unlink \n");
		}
	}
}

