#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "invertedIndex.h"

rootNode* createRoot(){
	rootNode* root = malloc(sizeof(rootNode));
	root->start = NULL;
	return root;
}

//populate trie with each word of each document
void populateTrie(rootNode* root,mapIndex* index,int noElems,generalInfo* info){
	char* tempWord = NULL;
	int counterWords = 0;
	for(int i=0;i<noElems;i++){
		int wordsSpecificDoc = 0;
		char* tempDocument = malloc((strlen(index[i].document)+1)*sizeof(char));
		strcpy(tempDocument,index[i].document);
		if(tempDocument == NULL)
			continue;
		
		tempWord = strtok(tempDocument," \t");
		while(tempWord!=NULL){
			//insert to trie
			counterWords++;
			wordsSpecificDoc++;
			insertTrie(tempWord,i,root);
			tempWord = strtok(NULL," \t");
		}
		index[i].words = wordsSpecificDoc;
		free(tempDocument);
		tempDocument = NULL;
	}
	info->totalWords = counterWords;
}

headQueue* createHeadQueue(){
	headQueue* head = malloc(sizeof(headQueue));
	head->firstNode = NULL;
	head->lastNode = NULL;
	head->size = 0;
}


trieNode* createNode(char c,int firstChar){
	trieNode* node = malloc(sizeof(trieNode));
	node->character = c;
	node->nextNode = NULL;
	node->head = NULL;
	node->postList = NULL;
	node->documentFrequency = 0;
	node->firstChar = firstChar;
	return node;
}

int insertPostList(postingLists** list,int id){
	if(*list == NULL){
		*list = malloc(sizeof(postingLists));
		(*list)->id = id;
		(*list)->timesAppeared = 1;
		(*list)->next = NULL;
		return 1;
	}else{
		postingLists* tempNode = *list;
		postingLists* lastNode = NULL;
		while(tempNode != NULL){
			lastNode = tempNode;
			if(tempNode->id == id){
				tempNode->timesAppeared++;
				return 0;
			}else{
				tempNode = tempNode->next;
			}
		}
		
		if(tempNode == NULL){
			lastNode->next = malloc(sizeof(postingLists));
			lastNode->next->id = id;
			lastNode->next->timesAppeared = 1;
			lastNode->next->next = NULL;
			return 1;
		}
	}
}


void insertTrie(char* word,int id,rootNode* root){
	if(root == NULL){
		root = createRoot();
	}
	
	if(root->start == NULL){
		root->start = createHeadQueue();
	}
	
	headQueue* head = root->start;
	for(int i=0; i<strlen(word); i++){
		char currentChar = word[i];
		int lastChar = 0;
		int firstChar = 0;
		if(i == strlen(word)-1)
			lastChar = 1;
		if(i==0){
			firstChar = 1;
		}
		head = insertCharacter(head,currentChar,id,lastChar,firstChar);
	}
}

//returns the next head of the trie in which we need to insert char
headQueue* insertCharacter(headQueue* wordQueue,char c,int id,int lastChar,int firstChar){
	//head null
	if(wordQueue == NULL){
		wordQueue = createHeadQueue();
		wordQueue->firstNode = createNode(c,firstChar);
		if(lastChar){
			int newNodeCreated = insertPostList(&wordQueue->firstNode->postList,id);
			if(newNodeCreated){
				wordQueue->firstNode->documentFrequency++;
			}
		}
		wordQueue->firstNode->head = createHeadQueue();
		wordQueue->lastNode = wordQueue->firstNode;
		wordQueue->size++;
		return wordQueue->firstNode->head;
	}
	
	//head has nodes
	trieNode* tempNode = wordQueue->firstNode;
	while(tempNode!=NULL){
		if(tempNode->character == c){
			if(lastChar){
				int newNodeCreated = insertPostList(&tempNode->postList,id);
				if(newNodeCreated){
					tempNode->documentFrequency++;
				}
			}
			return tempNode->head;
		}else{
			tempNode = tempNode->nextNode;
		}
	}

	//(firstNode is NULL)
	if(wordQueue->firstNode == NULL){
		wordQueue->firstNode = createNode(c,firstChar);
		if(lastChar){
			int newNodeCreated = insertPostList(&wordQueue->firstNode->postList,id);
			if(newNodeCreated){
				wordQueue->firstNode->documentFrequency++;
			}
		}
		wordQueue->firstNode->head = createHeadQueue();
		wordQueue->lastNode = wordQueue->firstNode;
		wordQueue->size++;
		return wordQueue->firstNode->head;
	}else{
		//has node but not the one we want->insert to lastNode->nextNode
		wordQueue->lastNode->nextNode = createNode(c,firstChar);
		if(lastChar){
			int newNodeCreated = insertPostList(&wordQueue->lastNode->nextNode->postList,id);
			if(newNodeCreated){
				wordQueue->lastNode->nextNode->documentFrequency++;
			}
		}
		wordQueue->lastNode->nextNode->head = createHeadQueue();
		wordQueue->lastNode = wordQueue->lastNode->nextNode;
		wordQueue->size++;
		return wordQueue->lastNode->head;
	}
}

void destroyInvertedIndex(rootNode** root){
	destroyHeadQueues(&(*root)->start);
	free(*root);
	*root = NULL;
}

void destroyPostList(trieNode** node){
	postingLists* currentNode = (*node)->postList;
	postingLists* nextNode;
	while (currentNode != NULL){
		nextNode = currentNode->next;
		free(currentNode);
		currentNode = nextNode;
	}
	(*node)->postList = NULL;
}

//recursively destroy lists
void destroyHeadQueues(headQueue** head){
	headQueue* currentHead = *head;
	trieNode* currentNode = currentHead->firstNode;
	trieNode* next;
	while(currentNode != NULL){	
		destroyHeadQueues(&currentNode->head);
		//delete node
		next = currentNode->nextNode;
		destroyPostList(&currentNode);
		free(currentNode);
		currentNode = next;
		currentHead->size--;
	}
	free(currentHead);
	currentHead = NULL;

}
