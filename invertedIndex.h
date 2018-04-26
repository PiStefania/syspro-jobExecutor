#ifndef _INVERTEDINDEX_H_
#define _INVERTEDINDEX_H_

#include "index.h"

typedef struct trieN trieNode;
typedef struct levelQueue headQueue;
typedef struct postingListsNode postingLists;

struct postingListsNode{
	int	line;
	int timesAppeared;		//term frequency
	postingLists* next;		
}postingListsNode;

typedef struct postingListsHead{
	char* fileName;
	int documentFrequency;		//size of postingLists
	postingLists* head;
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
void insertTrie(char* word,int id,rootNode* root);
headQueue* insertCharacter(headQueue* wordQueue,char c,int id,int lastChar,int firstChar);
trieNode* createNode(char c,int firstChar);
//int insertPostList(postingListsHead** list,int id);
void destroyInvertedIndex(rootNode** root);
//void destroyPostList(trieNode** node);
void destroyHeadQueues(headQueue** head);

#endif