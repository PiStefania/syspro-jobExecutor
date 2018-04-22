#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "invertedIndex.h"
#include "dfQuery.h"

int retDocFrequency(rootNode* root,char* word){
	if(root == NULL){
		return -1;
	}
	
	if(root->start == NULL){
		return -1;
	}
	
	trieNode* tempNode = root->start->firstNode;
	for(int i=0; i< strlen(word); i++){
		while(tempNode != NULL){
			if(tempNode->character == word[i]){
				if(i != strlen(word)-1){
					tempNode = tempNode->head->firstNode;
					break;
				}else{
					//found -> return document frequency
					return tempNode->documentFrequency;
				}
			}else{
				tempNode = tempNode->nextNode;
			}
		}
		if(tempNode == NULL){
			return -2;
		}
	}
}

//print modified stack
void DFS(trieNode* node,stack* stackWord){
	if (node != NULL){
		
		stackNode* pushNode = malloc(sizeof(stackNode));
		pushNode->alternatives = node->head->size;
		pushNode->character = node->character;
		if(node->postList != NULL){
			pushNode->lastChar = 1;
		}else{
			pushNode->lastChar = 0;
		}
		stackPush(stackWord,pushNode);
		free(pushNode);
		pushNode = NULL;
		if(node->postList != NULL){
			printCurrentStack(stackWord);
			printf(" %d\n",node->documentFrequency);
			//pop if character has been printed for all alternatives
			while(!stackIsEmpty(stackWord)){
				if(stackWord->elements[stackWord->top].alternatives == 0 && stackWord->elements[stackWord->top].lastChar){
					stackPop(stackWord);
				}else if(stackWord->elements[stackWord->top].alternatives == 1 && !stackWord->elements[stackWord->top].lastChar){
					stackPop(stackWord);
				}else{
					stackWord->elements[stackWord->top].alternatives--;
					break;
				}
			}
		}
		
		if (node->head->firstNode != NULL){
			DFS(node->head->firstNode,stackWord);
		}
		if (node->nextNode != NULL){
			DFS(node->nextNode,stackWord);
		}
	}
}


//stack's functions
void initializeStack(stack** stackWord,int size){
	(*stackWord)->elements = malloc(size*sizeof(stackNode));
	(*stackWord)->top = -1;
	(*stackWord)->size = size;
}

void destroyStack(stack** stackWord){
	free((*stackWord)->elements);
	(*stackWord)->elements = NULL;
	(*stackWord)->size = 0;
	(*stackWord)->top = -1;
	free(*stackWord);
	*stackWord = NULL;
}

int stackIsEmpty(stack* stackWord){
	return (stackWord->top < 0);
}

int stackIsFull(stack* stackWord){
	return (stackWord->top >= (stackWord->size - 1));
}

void doubleStack(stack* stackWord){
	int oldSize = stackWord->size;
	stackWord->elements = realloc(stackWord->elements,2*oldSize*sizeof(stackNode));
	stackWord->size = 2*oldSize;
}

void stackPush(stack* stackWord,stackNode* node){
	if(stackIsFull(stackWord)){
		doubleStack(stackWord);
	}
	
	stackWord->top++;
	stackWord->elements[stackWord->top] = *node;
}

stackNode* stackPop(stack* stackWord){	
	if(!stackIsEmpty(stackWord)){
		stackNode* elem = &stackWord->elements[stackWord->top];
		stackWord->top--;
		return elem;
	}
	return NULL;
}

void printStack(stack* stackWord){
	for(int i=stackWord->top;i>=0;i--){
		printf("%dth Elem: %c with alt: %d\n",i,stackWord->elements[i].character,stackWord->elements[i].alternatives);
	}
}

void printCurrentStack(stack* stackWord){
	for(int i=0; i <=stackWord->top; i++){
		printf("%c",stackWord->elements[i].character);
	}
}


