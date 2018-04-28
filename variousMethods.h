#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include "invertedIndex.h"

typedef struct pathsStruct{
	int noOfPaths;
	char** paths;
}pathsStruct;


void pickArgumentsMain(int argc,char* argv[],char** docfile,int* w);
pathsStruct* checkFileGetPaths(FILE *fp);
void optionsUserInput();
int returnPosWorker(int w,pid_t worker,pid_t* workers);

void printPaths(pathsStruct* p);
void destroyPathsStruct(pathsStruct** p);
void copyPaths(char** tempArray,char** array,int paths);
int countFileLines(FILE *fp);

#endif