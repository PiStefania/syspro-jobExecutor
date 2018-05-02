#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
#include "variousMethods.h"
#define DEF_SIZE 10

char* searchQueries(foundLines* array,int deadline){
	int deadLineLength = getNumberLength(deadline);
	int sumLength = strlen("search|") + deadLineLength + 1;
	for(int i=0;i<array->length;i++){
		sumLength += strlen(array->lines[i]);
	}
	
	char* sumStr = malloc((sumLength + 2)*sizeof(char));
	sprintf(sumStr,"search|%d ",deadline);
	for(int i=0;i<array->length;i++){
		strcat(sumStr,array->lines[i]);
	}
	return sumStr;
}

foundLines* combinedLines(searchInfo* arrayInfo,indexesArray* indexesArr){
	int initSize = 10;
	int nextSize = initSize;
	foundLines* array = malloc(sizeof(foundLines));
	array->lines = malloc(initSize*sizeof(char*));
	array->length = 0;
	int sizeInfo = arrayInfo->position;
	int sizeIndexes = indexesArr->length;
	int counter = 0;
	//find index with same fileName
	for(int i=0;i<sizeInfo;i++){
		for(int j=0;j<sizeIndexes;j++){
			if(strcmp(arrayInfo->nodes[i].fileName,indexesArr->indexes[j]->fileName)==0){
				//for each line
				for(int k=0;k<arrayInfo->nodes[i].noLines;k++){
					int temp = arrayInfo->nodes[i].lines[k];
					if(temp == -1)
						continue;
					if(k+1<arrayInfo->nodes[i].noLines){
						if(temp == arrayInfo->nodes[i].lines[k+1])
							continue;
					}
					if(counter == initSize-1){
						nextSize = 2* nextSize;
						array->lines = realloc(array->lines,nextSize*sizeof(char*));
					}
					char* line = indexesArr->indexes[j]->documents[temp];
					char* fileName = indexesArr->indexes[j]->fileName;
					int lengthLine = getNumberLength(temp);
					
					array->lines[counter] = malloc((strlen(line)+ 6 + lengthLine + strlen(fileName))*sizeof(char));
					sprintf(array->lines[counter],"%s %d %s\n",fileName,temp,line);
					counter++;
				}
			}
		}
	}
	
	array->length = counter;
	return array;
}

searchInfo* searchFiles(rootNode* root,char** searchWords,int words){
	if(root == NULL){
		return NULL;
	}

	if(root->start == NULL){
		return NULL;
	}
	
	//initialize searchInfo array
	searchInfo* arrayInfo = initializeArrayInfo();
	int length = 0;
	for(int j=0;j<words;j++){
		trieNode* tempNode = root->start->firstNode;
		char* word = searchWords[j];
		for(int i=0; i < strlen(word); i++){
			while(tempNode != NULL){
				if(tempNode->character == word[i]){
					if(i != strlen(word)-1){
						tempNode = tempNode->head->firstNode;
						break;
					}else{
						if(tempNode->postList != NULL){
							length += tempNode->postList->documentFrequency;
							createSearchNodesWord(arrayInfo,root,word);
							break;
						}
					}
				}else{
					tempNode = tempNode->nextNode;
				}
			}
			if(tempNode == NULL){
				break;
			}
		}
	}
	
	if(length == 0){
		if(arrayInfo->position == 0){
			free(arrayInfo->nodes);
			arrayInfo->nodes = NULL;
			free(arrayInfo);
			arrayInfo = NULL;
		}
		return NULL;
	}else{
		return arrayInfo;
	}
	
}

void createSearchNodesWord(searchInfo* arrayInfo,rootNode* root,char* word){
	int length = 0;
	int found = 0;
	trieNode* tempNode = root->start->firstNode;
	for(int i=0; i < strlen(word); i++){
		while(tempNode != NULL){
			if(tempNode->character == word[i]){
				if(i != strlen(word)-1){
					tempNode = tempNode->head->firstNode;
					break;
				}else{
					if(tempNode->postList != NULL){
						length = tempNode->postList->documentFrequency;
						int found = 1;
						break;
					}
				}
			}else{
				tempNode = tempNode->nextNode;
			}
		}
		if(found){
			break;
		}
		if(tempNode == NULL){
			break;
		}
	}
	
	if(length != 0){
		postingLists* temp = tempNode->postList->firstNode;
		while(temp!=NULL){
			//search filename before creating new nodes
			int search = searchFileName(arrayInfo,temp->fileName);
			if(search >= 0){
				//fileName exists
				//add additional lines
				int noLines = arrayInfo->nodes[search].noLines;
				arrayInfo->nodes[search].lines = realloc(arrayInfo->nodes[search].lines,(noLines+temp->linesArr->length)*sizeof(int));
				memcpy(&arrayInfo->nodes[search].lines[noLines], temp->linesArr->lines,temp->linesArr->length*sizeof(int));
				arrayInfo->nodes[search].noLines += temp->linesArr->length;
				heapSort(arrayInfo->nodes[search].lines,arrayInfo->nodes[search].noLines);
			}else{
				//create a new node
				searchNode* node =  malloc(sizeof(searchNode));
				node->fileName = malloc((strlen(temp->fileName)+1)*sizeof(char));
				strcpy(node->fileName,temp->fileName);
				node->lines = malloc(temp->linesArr->length*sizeof(int));
				memcpy(node->lines, temp->linesArr->lines,temp->linesArr->length*sizeof(int));
				node->noLines = temp->linesArr->length;
				arrayInfo->nodes[arrayInfo->position] = *node;
				heapSort(arrayInfo->nodes[arrayInfo->position].lines,arrayInfo->nodes[arrayInfo->position].noLines);
				arrayInfo->position++;
				free(node);
			}
			temp = temp->next;
		}
	}
}



int searchFileName(searchInfo* arrayInfo,char* fileName){
	if(arrayInfo->position != 0){
		for(int i=0;i<arrayInfo->position;i++){
			if(strcmp(arrayInfo->nodes[i].fileName,fileName)==0){
				return i;
			}
		}
	}
	return -1;
}


searchInfo* initializeArrayInfo(){
	searchInfo* arrayInfo = malloc(sizeof(searchInfo));
	arrayInfo->position = 0;
	arrayInfo->length = DEF_SIZE;
	arrayInfo->nodes = malloc(DEF_SIZE*sizeof(searchNode));
	for(int i=0;i<DEF_SIZE;i++){
		arrayInfo->nodes[i].fileName = NULL;
		arrayInfo->nodes[i].lines = NULL;
		arrayInfo->nodes[i].noLines = 0;
	}
	return arrayInfo;
}

void doubleArrayInfo(searchInfo* arrayInfo){
	if(arrayInfo->position==arrayInfo->length-1){
		int oldSize = arrayInfo->length;
		arrayInfo->nodes = realloc(arrayInfo->nodes,oldSize*2*sizeof(searchNode));
		arrayInfo->length = 2*oldSize;
		for(int i=oldSize;i<arrayInfo->length;i++){
			arrayInfo->nodes[i].fileName = NULL;
			arrayInfo->nodes[i].lines = NULL;
			arrayInfo->nodes[i].noLines = 0;
		}
	}
}

void destroyArrayInfo(searchInfo* arrayInfo){
	for(int i=0;i<arrayInfo->position;i++){
		if(arrayInfo->nodes[i].fileName!=NULL){
			free(arrayInfo->nodes[i].fileName);
			arrayInfo->nodes[i].fileName = NULL;
		}
		if(arrayInfo->nodes[i].lines!=NULL){
			free(arrayInfo->nodes[i].lines);
			arrayInfo->nodes[i].lines = NULL;
		}
	}
	free(arrayInfo->nodes);
	arrayInfo->nodes = NULL;
	free(arrayInfo);
	arrayInfo = NULL;
}

void printSearchInfo(searchInfo* arrayInfo){
	for(int i=0;i<arrayInfo->position;i++){
		printf("file: %s, lines: %d, ",arrayInfo->nodes[i].fileName,arrayInfo->nodes[i].noLines);
		for(int j=0;j<arrayInfo->nodes[i].noLines;j++){
			printf("|%d|",arrayInfo->nodes[i].lines[j]);
		}
		printf("\n");
	}
}

void heapify(int* arr, int n, int i){
	int largest = i;  // Initialize largest as root
    int l = 2*i + 1;  // left = 2*i + 1
    int r = 2*i + 2;  // right = 2*i + 2

    if (l < n && arr[l] > arr[largest])
        largest = l;

    if (r < n && arr[r] > arr[largest])
        largest = r;

    // If largest is not root
    if (largest != i)
    {
        int temp = arr[i];
		arr[i] = arr[largest];
		arr[largest] = temp;
		heapify(arr, n, largest);
    }
}

void buildHeap(int* arr,int length){
	for(int i = length / 2; i >= 0; i--){
		heapify(arr,length,i);
	}
}

void heapSort(int* arr,int length){
    buildHeap(arr,length);
    for (int i=length-1; i>=0; i--)
    {
        int temp = arr[0];
		arr[0] = arr[i];
		arr[i] = temp;
        heapify(arr, i, 0);
    }
}

void destroyFoundLines(foundLines* array){
	for(int i=0;i<array->length;i++){
		if(array->lines[i]!=NULL){
			free(array->lines[i]);
			array->lines[i] = NULL;
		}
	}
	free(array->lines);
	array->lines = NULL;
	free(array);
	array = NULL;
}
