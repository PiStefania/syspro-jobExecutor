#ifndef _INVERTEDINDEX_H_
#define _INVERTEDINDEX_H_

#include "index.h"

typedef struct trieN trieNode;
typedef struct levelQueue headQueue;
typedef struct postingListsNode postingLists;

typedef struct linesArray{
	int* lines;
	int length;
}linesArray;

struct postingListsNode{
	char* fileName;
	pid_t worker;
	linesArray* linesArr;
	int timesAppeared;		//term frequency
	postingLists* next;		
}postingListsNode;

typedef struct postingListsHead{
	int documentFrequency;		//size of postingLists
	postingLists* firstNode;
}postingListsHead;

struct trieN{
	char character;
	trieNode *nextNode;
	headQueue *head;
	postingListsHead* postList;
	int firstChar;				//be the first character of a word
}trieN;


struct levelQueue{
	trieNode* firstNode;
	trieNode* lastNode;
	int size;				//size of list headQueue
}levelQueue;

typedef struct rootNode{
	headQueue* start;
}rootNode;

rootNode* createRoot();
headQueue* createHeadQueue();
void populateTrie(rootNode* root,mapIndex* index,int noElems);
void insertTrie(char* word,char* fileName,int line,rootNode* root);
headQueue* insertCharacter(headQueue* wordQueue,char c,char* fileName,int line,int lastChar,int firstChar);
trieNode* createNode(char c,int firstChar);
void insertPostList(postingListsHead** head,char* fileName,int line);
void destroyInvertedIndex(rootNode** root);
void destroyPostList(trieNode** node);
void destroyHeadQueues(headQueue** head);
void destroyPostingListNode(postingLists** node);

//linesArray functions
linesArray* initializeArrayLines();
int getNextEmptyPosLine(linesArray* linesArr);
void doubleLinesArray(linesArray* linesArr);

#endif