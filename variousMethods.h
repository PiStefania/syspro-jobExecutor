#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

//#include "invertedIndex.h"

typedef struct pathsStruct{
	int noOfPaths;
	char** paths;
}pathsStruct;


void pickArgumentsMain(int argc,char* argv[],char** docfile,int* w);
pathsStruct* checkFileGetPaths(FILE *fp);
//void optionsUserInput(int K,rootNode* root,generalInfo* info,mapIndex* index);

void printPaths(pathsStruct* p);
void destroyPathsStruct(pathsStruct** p);

#endif