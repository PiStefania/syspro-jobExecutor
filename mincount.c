#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maxcount.h"
#include "mincount.h"

//find word and return document's mincount info
minCountInfo* returnMinCount(rootNode* root,char* word){
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
						return searchPostListMin(tempNode->postList);
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

//searches a postingLists list in order to find a node with min timesAppeared
minCountInfo* searchPostListMin(postingListsHead* list){
	minCountInfo* info = malloc(sizeof(minCountInfo));
	postingLists* tempNode = list->firstNode;
	postingLists* minNode = tempNode;
	int minCount = maxCount(list);
	while(tempNode != NULL){
		if(minCount > tempNode->timesAppeared){
			minNode = tempNode;
			minCount = tempNode->timesAppeared;
		}
		else if(minCount ==  tempNode->timesAppeared){
			if(strcmp(minNode->fileName,tempNode->fileName) > 0){
				minNode = tempNode;
			}
		}
		tempNode = tempNode->next;
	}

	info->fileName = malloc((strlen(minNode->fileName)+1)*sizeof(char));
	strcpy(info->fileName,minNode->fileName);
	info->timesAppeared = minNode->timesAppeared;
	
	return info;
}

void destroyMinCountInfo(minCountInfo* info){
	free(info->fileName);
	info->fileName = NULL;
	free(info);
	info = NULL;
}