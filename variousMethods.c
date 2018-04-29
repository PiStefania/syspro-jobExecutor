#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "variousMethods.h"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#define MAX_WORDS 10
#define STACK_SIZE 10

//specifies whether or not the arguments given are correct
void pickArgumentsMain(int argc,char* argv[],char** docfile,int* w){
	
	char* numOfWorkers = NULL;
	
	if(argc == 5){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-d")==0)
				*docfile=argv[i+1];			
			if(strcmp(argv[i],"-w")==0)
				numOfWorkers=argv[i+1];
		}
	}
	else if(argc == 3){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-d")==0)
				*docfile=argv[i+1];			
		}
	}
	else{
		printf("Wrong number of arguments. Terminate process.\n");
		exit(1);
	}
	
	if(*docfile == NULL){
		printf("Error! Wrong Arguments!\n");
		exit(1);
	}
	
	if(numOfWorkers == NULL && argc == 3){
		*w = 1;
	}else if(numOfWorkers == NULL && argc == 5){
		printf("Error getting value of w. Terminate process.\n");
		exit(1);
	}else{
		*w = atoi(numOfWorkers);
	}
	
	if(*w == 0){
		printf("Error w = 0. Terminate process.\n");
		exit(1);
	}
}

//checks file and returns array of paths
pathsStruct* checkFileGetPaths(FILE *fp){
	int read;
	size_t len = 0;
	int lines = 0;
	DIR* dirDesc;
	int pathsExist = 0;
	//change after
	char* line = NULL;
	while((read = getline(&line, &len, fp)) != -1){
		//check line whitespace
		char* temp = strtok(line,"\n");
		if(temp == NULL){
			printf("Line %d is empty. Continue.",lines);
			continue;
		}
		temp = strtok(temp," \t");
		if(temp == NULL){
			printf("Line %d is empty. Continue.",lines);
			continue;
		}
		
		//check if path exists
		dirDesc = opendir(line);
		if(dirDesc == NULL){
			printf("Path in line %d doesn't exist.\n",lines);
		}else{
			pathsExist = 1;
			lines++;
			closedir(dirDesc);
		}
	}
	if(line){
		free(line);
		line = NULL;
	}
	
	pathsStruct* p = malloc(sizeof(pathsStruct)); 
	p->paths = malloc(lines*sizeof(char*));
	p->noOfPaths = lines;
	if(pathsExist){
		rewind(fp);
		int i=0;
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

			//check if path exists
			dirDesc = opendir(line);
			if(dirDesc != NULL){
				p->paths[i] = malloc((strlen(line)+1)*sizeof(char));
				strcpy(p->paths[i],line);
				closedir(dirDesc);
				i++;
			}
		}
		
		if(line){
			free(line);
			line = NULL;
		}
	}else{
		return NULL;
	}
	return p;
}

void destroyPathsStruct(pathsStruct** p){
	for(int i=0;i<(*p)->noOfPaths;i++){
		free((*p)->paths[i]);
		(*p)->paths[i] = NULL;
	}
	free((*p)->paths);
	(*p)->paths = NULL;
	free((*p));
	*p = NULL;
}

void printPaths(pathsStruct* p){
	for(int i=0;i<p->noOfPaths;i++){
		printf("%s\n",p->paths[i]);
	}
}

void copyPaths(char** tempArray,char** array,int paths){
	for(int i=0;i<paths;i++){
		tempArray[i] = malloc((strlen(array[i])+1)*sizeof(char));
		strcpy(tempArray[i],array[i]);
	}
}

int countFileLines(FILE *fp){
	char ch;
	int lines = 0;
	while(!feof(fp))
	{
		ch = fgetc(fp);
		if(ch == '\n')
		{
			lines++;
		}
	}
	lines++;
	return lines;
}

int returnPosWorker(int w,pid_t worker,pid_t* workers){
	for(int i=0;i<w;i++){
		if(workers[i]==worker){
			return i;
		}
	}
	return -1;
}

int getNumberLength(int no){
	int numberLength;
	if(no == 0){
		numberLength = 1;
	}else{
		numberLength = floor(log10(abs(no))) + 1;
	}
	
	return numberLength;
}

