#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "invertedIndex.h"

#define DEFAULT_LINES 5

//creates root of trie
rootNode* createRoot(){
	rootNode* root = malloc(sizeof(rootNode));
	root->start = NULL;
	return root;
}

//populate trie with each word of each document
void populateTrie(rootNode* root,mapIndex* index,int noElems){
	char* tempWord = NULL;
	int counterWords = 0;
	char* fileName = index->fileName;
	for(int i=0;i<noElems;i++){
		int wordsSpecificDoc = 0;
		char* tempDocument = malloc((strlen(index->documents[i])+1)*sizeof(char));
		strcpy(tempDocument,index->documents[i]);
		if(tempDocument == NULL)
			continue;
		
		tempWord = strtok(tempDocument," \t");
		while(tempWord!=NULL){
			//insert to trie
			counterWords++;
			wordsSpecificDoc++;
			insertTrie(tempWord,fileName,i,root);
			tempWord = strtok(NULL," \t");
		}
		index->words += wordsSpecificDoc;
		free(tempDocument);
		tempDocument = NULL;
	}
}

//create horizontal list
headQueue* createHeadQueue(){
	headQueue* head = malloc(sizeof(headQueue));
	head->firstNode = NULL;
	head->lastNode = NULL;
	head->size = 0;
}

//create node for trie
trieNode* createNode(char c,int firstChar){
	trieNode* node = malloc(sizeof(trieNode));
	node->character = c;
	node->nextNode = NULL;
	node->head = NULL;
	node->postList = NULL;
	node->firstChar = firstChar;
	return node;
}

//initialize lines array
linesArray* initializeArrayLines(){
	linesArray* linesArr = malloc(sizeof(linesArray));
	linesArr->length = DEFAULT_LINES;
	linesArr->lines = malloc(linesArr->length*sizeof(int));
	for(int i=0;i<linesArr->length;i++){
		linesArr->lines[i] = -1;	
	}
	return linesArr;
}

//get next empty position of lines array
int getNextEmptyPosLine(linesArray* linesArr){
	for(int i=0;i<linesArr->length;i++){
		if(linesArr->lines[i]==-1){
			return i;
		}
	}
	return -1;
}

//double lines array when full
void doubleLinesArray(linesArray* linesArr){
	int nextPos = getNextEmptyPosLine(linesArr);
	if(nextPos==-1){
		int oldSize = linesArr->length;
		linesArr->lines = realloc(linesArr->lines,(oldSize*2)*sizeof(int));
		linesArr->length = 2*oldSize;
		for(int i=oldSize;i<linesArr->length;i++){
			linesArr->lines[i] = -1;	
		}
	}
}

//insert a node to postingLists list
void insertPostList(postingListsHead** head,char* fileName,int line){
	if(*head == NULL){
		//empty list
		*head = malloc(sizeof(postingListsHead));
		(*head)->documentFrequency = 1;
		(*head)->firstNode = malloc(sizeof(postingLists));
		(*head)->firstNode->timesAppeared = 1;
		(*head)->firstNode->next = NULL;
		(*head)->firstNode->fileName = malloc((strlen(fileName)+1)*sizeof(char));
		strcpy((*head)->firstNode->fileName,fileName);
		(*head)->firstNode->linesArr = initializeArrayLines();
		(*head)->firstNode->linesArr->lines[0] = line;
		(*head)->firstNode->worker = getpid();
	}else{
		//search for same
		postingLists* tempNode = (*head)->firstNode;
		postingLists* lastNode = NULL;
		while(tempNode != NULL){
			lastNode = tempNode;
			if(strcmp(tempNode->fileName,fileName)==0){
				tempNode->timesAppeared++;
				int newPos = getNextEmptyPosLine(tempNode->linesArr);
				if(newPos == -1){
					int oldSize = tempNode->linesArr->length;
					doubleLinesArray(tempNode->linesArr);
					tempNode->linesArr->lines[oldSize] = line;
				}else{
					tempNode->linesArr->lines[newPos] = line;
				}
				return;
			}else{
				tempNode = tempNode->next;
			}
		}
		
		//not found -> add a new node
		if(tempNode == NULL){
			(*head)->documentFrequency++;
			lastNode->next = malloc(sizeof(postingLists));
			lastNode->next->fileName = malloc((strlen(fileName)+1)*sizeof(char));
			strcpy(lastNode->next->fileName,fileName);
			lastNode->next->linesArr = initializeArrayLines();
			lastNode->next->linesArr->lines[0] = line;
			lastNode->next->timesAppeared = 1;
			lastNode->next->next = NULL;
			lastNode->next->worker = getpid();
		}
	}
}

//insert a word to trie
void insertTrie(char* word,char* fileName,int line,rootNode* root){
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
		head = insertCharacter(head,currentChar,fileName,line,lastChar,firstChar);
	}
}

//returns the next head of the trie in which we need to insert char
headQueue* insertCharacter(headQueue* wordQueue,char c,char* fileName,int line,int lastChar,int firstChar){
	//head null
	if(wordQueue == NULL){
		wordQueue = createHeadQueue();
		wordQueue->firstNode = createNode(c,firstChar);
		if(lastChar){
			insertPostList(&wordQueue->firstNode->postList,fileName,line);
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
				insertPostList(&tempNode->postList,fileName,line);
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
			insertPostList(&wordQueue->firstNode->postList,fileName,line);
		}
		wordQueue->firstNode->head = createHeadQueue();
		wordQueue->lastNode = wordQueue->firstNode;
		wordQueue->size++;
		return wordQueue->firstNode->head;
	}else{
		//has node but not the one we want->insert to lastNode->nextNode
		wordQueue->lastNode->nextNode = createNode(c,firstChar);
		if(lastChar){
			insertPostList(&wordQueue->lastNode->nextNode->postList,fileName,line);
		}
		wordQueue->lastNode->nextNode->head = createHeadQueue();
		wordQueue->lastNode = wordQueue->lastNode->nextNode;
		wordQueue->size++;
		return wordQueue->lastNode->head;
	}
}

//destroys trie
void destroyInvertedIndex(rootNode** root){
	destroyHeadQueues(&(*root)->start);
	free(*root);
	*root = NULL;
}

//destroys postList
void destroyPostList(trieNode** node){
	if((*node)->postList != NULL){
		postingLists* currentNode = (*node)->postList->firstNode;
		postingLists* nextNode;
		while (currentNode != NULL){
			nextNode = currentNode->next;
			destroyPostingListNode(&currentNode);
			currentNode = nextNode;
		}

		free((*node)->postList);
		(*node)->postList = NULL;
	}
}

//destroys a node ftom postingLists
void destroyPostingListNode(postingLists** node){
	free((*node)->linesArr->lines);
	(*node)->linesArr->lines = NULL;
	free((*node)->linesArr);
	(*node)->linesArr = NULL;
	free((*node)->fileName);
	(*node)->fileName = NULL;
	free((*node));
	*node = NULL;
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
