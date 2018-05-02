#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "invertedIndex.h"
#include "worker.h"

typedef struct searchNode{
	char* fileName;
	int* lines;
	int noLines;
}searchNode;

typedef struct searchInfo{
	searchNode* nodes;
	int length;
	int position;
}searchInfo;

typedef struct foundLines{
	int length;
	char** lines;
}foundLines;

char* searchQueries(foundLines* array,int deadline);
foundLines* combinedLines(searchInfo* arrayInfo,indexesArray* indexesArr);
searchInfo* searchFiles(rootNode* root,char** searchWords,int words);
void createSearchNodesWord(searchInfo* nodes,rootNode* root,char* word);
int searchFileName(searchInfo* arrayInfo,char* fileName);

//for structures
searchInfo* initializeArrayInfo();
void doubleArrayInfo(searchInfo* arrayInfo);
void destroyArrayInfo(searchInfo* arrayInfo);
void printSearchInfo(searchInfo* arrayInfo);
void destroyFoundLines(foundLines* array);

void heapify(int* arr, int n, int i);
void buildHeap(int* arr,int length);
void heapSort(int* arr,int length);

#endif