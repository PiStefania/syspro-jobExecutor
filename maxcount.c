#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maxcount.h"

//find word and return document's maxcount info
maxCountInfo* returnMaxCount(rootNode* root,char* word){
	if(root == NULL){
		return NULL;
	}
	
	if(root->start == NULL){
		return NULL;
	}
	
	trieNode* tempNode = root->start->firstNode;
	for(int i=0; i < strlen(word); i++){
		while(tempNode != NULL){
			if(tempNode->character == word[i]){
				if(i != strlen(word)-1){
					tempNode = tempNode->head->firstNode;
					break;
				}else{
					if(tempNode->postList != NULL){
						return searchPostListMax(tempNode->postList);
					}
					break;
				}
			}else{
				tempNode = tempNode->nextNode;
			}
		}
		if(tempNode == NULL){
			return NULL;
		}
	}
	return NULL;
}

//searches a postingLists list in order to find a node with max timesAppeared
maxCountInfo* searchPostListMax(postingListsHead* list){
	maxCountInfo* info = malloc(sizeof(maxCountInfo));
	postingLists* tempNode = list->firstNode;
	postingLists* maxNode = NULL;
	int maxCount = 0;
	while(tempNode != NULL){
		if(maxCount < tempNode->timesAppeared){
			maxNode = tempNode;
			maxCount = tempNode->timesAppeared;
		}
		else if(maxCount ==  tempNode->timesAppeared){
			if(strcmp(maxNode->fileName,tempNode->fileName) > 0){
				maxNode = tempNode;
			}
		}
		tempNode = tempNode->next;
	}
	
	info->fileName = malloc((strlen(maxNode->fileName)+1)*sizeof(char));
	strcpy(info->fileName,maxNode->fileName);
	info->timesAppeared = maxNode->timesAppeared;
	
	return info;
}

int maxCount(postingListsHead* list){
	postingLists* tempNode = list->firstNode;
	int maxCount = 0;
	while(tempNode != NULL){
		if(maxCount < tempNode->timesAppeared){
			maxCount = tempNode->timesAppeared;
		}
		tempNode = tempNode->next;
	}
	
	return maxCount;
}

void destroyMaxCountInfo(maxCountInfo* info){
	free(info->fileName);
	info->fileName = NULL;
	free(info);
	info = NULL;
}