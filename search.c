#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
#define DEF_SIZE 10

char* searchFiles(rootNode* root,char** searchWords,int words){
	if(root == NULL){
		return NULL;
	}

	if(root->start == NULL){
		return NULL;
	}
	
	int lengthPostingLists = 0;
	trieNode* tempNode = root->start->firstNode;
	for(int j=0;j<words;j++){
		char* word = searchWords[j];
		for(int i=0; i < strlen(word); i++){
			while(tempNode != NULL){
				if(tempNode->character == word[i]){
					if(i != strlen(word)-1){
						tempNode = tempNode->head->firstNode;
						break;
					}else{
						if(tempNode->postList != NULL){
							lengthPostingLists += tempNode->postList->documentFrequency;
						}
					}
				}else{
					tempNode = tempNode->nextNode;
				}
			}
			if(tempNode == NULL){
				return NULL;
			}
		}
	}
	
	if(lengthPostingLists == 0){
		return NULL;
	}else{
	}
	
}

searchInfo* createSearchNodesWord(searchInfo* nodes,rootNode* root,char* word,char** fileNames){
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
	
	if(tempNode == NULL || length == 0){
		return NULL;
	}else{
		//search filename before creating new nodes
		int search = 
		if(search){
			//fileName exists
			//add additional lines
			
		}else{
			//create a new node
		}
		
		nodes = malloc(length*sizeof(searchInfo));
		postingLists* temp = tempNode->postList->firstNode;
		for(int i=0;i<length,temp!=NULL;i++){
			nodes[i].fileName = malloc((strlen(temp->fileName)+1)*sizeof(char));
			strcpy(nodes[i].fileName,temp->fileName);
			nodes[i].lines = malloc(temp->linesArr->length*sizeof(int));
			memcpy(nodes[i].lines, temp->linesArr->lines,temp->linesArr->length*sizeof(int));
			temp = temp->next;
		}
		return nodes;
	}
}



int searchFileName(searchInfo* nodes,char* fileName){
	
}

searchInfo* initializeArrayInfo(){
	
}
