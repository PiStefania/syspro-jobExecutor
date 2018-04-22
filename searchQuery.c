#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>
#include <inttypes.h>
#include "searchQuery.h"
#include "generalInfo.h"
#define INIT_SIZE 10
#define MAX_CHARS 14000
#define DEFAULT_MAX 136

//print max k scores 
void printMaxKScores(scores* scoresArray,mapIndex* index,int K){
	for(int i=0;i<K;i++){
		printf("%d.(%d)[%lf] ",i+1,scoresArray->scoreArray[i].id,scoresArray->scoreArray[i].score);
		ssize_t bufSize = snprintf(NULL,0,"%d.(%d)[%lf] ",i+1,scoresArray->scoreArray[i].id,scoresArray->scoreArray[i].score);
		printAlteredOutput(&scoresArray->scoreArray[i],index,bufSize);
	}
}

void printAlteredOutput(scoreNode* scoreArray,mapIndex* index,ssize_t bufSize){
	int documentLength = strlen(index[scoreArray->id].document);
	int newLine;
	//get terminal info
	struct winsize w;
    int flag = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int maxChars;
	if(flag != 0){
		maxChars = DEFAULT_MAX;
	}else{
		maxChars = w.ws_col;
		if(maxChars > DEFAULT_MAX){
			maxChars = DEFAULT_MAX;
		}
	} 
	int totalChars = documentLength + bufSize;
	newLine = totalChars / maxChars;
	if(newLine == 0){
		//document can be printed on one line
		char* underArray = malloc((documentLength+1)*sizeof(char));
		initializeUnderArray(underArray,documentLength+1);
		//print new line and search words for underlining
		printf("%s\n",index[scoreArray->id].document);
		char* wordsOfDocument = malloc((strlen(scoreArray->words)+1)*sizeof(char));
		strcpy(wordsOfDocument,scoreArray->words);
		char* subString = strtok(wordsOfDocument," ");
		int subStringLength = strlen(subString);
		while(wordsOfDocument != NULL){
			char* ptr = index[scoreArray->id].document;
			while((ptr=strstr(ptr,subString)) != NULL){
				//for each first position of char of occurence
				unsigned int position = (uintptr_t) ptr - (uintptr_t) index[scoreArray->id].document;
				//check if it is a word
				if(checkIfWord(index[scoreArray->id].document,position,subStringLength)){
					insertSpecialChars(underArray,position,position+subStringLength);
				}
				//search from next position
				ptr++;
			}
			subString = strtok(NULL," ");
			if(subString == NULL)
				break;
			subStringLength = strlen(subString);
		}
		printSpaces(bufSize);
		printUnderArray(underArray,documentLength+1);
		free(underArray);
		underArray = NULL;
		free(wordsOfDocument);
		wordsOfDocument = NULL;
		printf("\n");
	}else{
		//document should be printed in mupliple lines
		int posPrinted = 0;
		int lengthLine = maxChars - bufSize;
		int partSize = documentLength / (newLine + 1);
		printDividedLines(index[scoreArray->id].document,documentLength,partSize,scoreArray,bufSize);
	}
}

//print " " depending on 'spaces'
void printSpaces(int spaces){
	for(int i=0;i<spaces;i++){
		printf(" ");
	}
}

//initialize under array
void initializeUnderArray(char* underArray,int length){
	for(int i=0;i<length;i++){
		underArray[i] = ' ';
	}
}

//inserts underlying special char '^' depending on 'lengthWord'
void insertSpecialChars(char* underArray,int beginning,int lengthWord){
	for(int i=beginning;i<lengthWord;i++){
		underArray[i] = '^';
	}
}

void printUnderArray(char* underArray,int length){
	for(int i=0;i<length;i++){
		printf("%c",underArray[i]);
	}
}

//checks if a substring is a word
int checkIfWord(char* string,int position,int wordLength){
	if(position > 0){
		if(isspace(string[position-1]) && isspace(string[position+wordLength]))
			return 1;
		else return 0;
	}else{
		if(isspace(string[position+wordLength]))
		   return 1;
		else return 0;
	}	
}

//checks if a position in a document is broken from breaking the document in lines
int checkCleanWord(char* source,int c){
	int length = strlen(source);
	if(isspace(source[c]) || c == length){
		//whitespace
		return 0;
	}else{
		if(isspace(source[c+1]) ||  c+1 == length)
			return 0;
		int i = c;
		int counter = 0;
		while(!isspace(source[i])){
			counter++;
			i--;
		}
		return counter;
	}
}

//prints mupliple lines
void printDividedLines(char* source,int documentLength,int partLength,scoreNode* scoreArray,int bufSize){
	char* line;
	int size = 0;
	int counterLength = partLength;
	int foundWord = 0;
	for(int i=0;i<documentLength;i++){
		size++;
		if(i == counterLength){
			//not last part of document
			int breakWord = checkCleanWord(source,i);
			if(breakWord > 0){
				size -= breakWord;
				i -= breakWord;
			}
			line = malloc((size+1)*sizeof(char));
			strncpy(line,source + i - size + 1,size);
			line[size] = '\0';			
			//document can be printed on one line
			char* underArray = malloc((size+1)*sizeof(char));
			initializeUnderArray(underArray,size);
			//print new line and search words for underlining
			printf("%s\n",line);
			printSpaces(bufSize);
			char* wordsOfDocument = malloc((strlen(scoreArray->words)+1)*sizeof(char));
			strcpy(wordsOfDocument,scoreArray->words);
			char* subString = strtok(wordsOfDocument," ");
			int subStringLength = strlen(subString);
			while(wordsOfDocument != NULL){
				char* ptr = line;
				while((ptr=strstr(ptr,subString)) != NULL){
					//for each first position of char of occurence
					unsigned int position = (uintptr_t) ptr - (uintptr_t) line;
					//check if it is a word
					if(checkIfWord(line,position,subStringLength)){
						insertSpecialChars(underArray,position,position+subStringLength);
						foundWord = 1;
					}
					//search from next position
					ptr++;
				}
				subString = strtok(NULL," ");
				if(subString == NULL)
					break;
				subStringLength = strlen(subString);
			}
			if(foundWord){
				printUnderArray(underArray,size);
				printf("\n");
				printSpaces(bufSize);
			}
			free(underArray);
			underArray = NULL;
			free(wordsOfDocument);
			wordsOfDocument = NULL;
			
			size = 0;
			foundWord = 0;

			free(line);
			line = NULL;
			if(counterLength + partLength > documentLength){
				counterLength += documentLength - counterLength;
			}else{
				counterLength += partLength;
			}
		}
		if(i == documentLength-1){
			//last part of document
			int breakWord = checkCleanWord(source,i);
			if(breakWord > 0){
				size -= breakWord;
			}
			line = malloc((size+1)*sizeof(char));
			strncpy(line,source + i - size + 1,size);
			line[size] = '\0';

			//print special chars
			char* underArray = malloc((size+1)*sizeof(char));
			initializeUnderArray(underArray,size+1);
			printf("%s\n",line);
			printSpaces(bufSize);
			//print new line and search words underline
			char* wordsOfDocument = malloc((strlen(scoreArray->words)+1)*sizeof(char));
			strcpy(wordsOfDocument,scoreArray->words);
			char* subString = strtok(wordsOfDocument," ");
			int subStringLength = strlen(subString);
			while(wordsOfDocument != NULL){
				char* ptr = line;
				while((ptr=strstr(ptr,subString)) != NULL){
					//for each first position of char of occurence
					unsigned int position = (uintptr_t) ptr - (uintptr_t) line;
					if(checkIfWord(line,position,subStringLength)){
						foundWord = 1;
						insertSpecialChars(underArray,position,position+subStringLength);
					}
					ptr++;
				}
				subString = strtok(NULL," ");
				if(subString == NULL)
					break;
				subStringLength = strlen(subString);
			}
			if(foundWord){
				printUnderArray(underArray,size);
			}
			printf("\n");
			free(underArray);
			underArray = NULL;
			free(wordsOfDocument);
			wordsOfDocument = NULL;
			free(line);
			line = NULL;
		}
	}
}


void heapify(scores* scoresArray, int n, int i){
	int largest = i;  // Initialize largest as root
    int l = 2*i + 1;  // left = 2*i + 1
    int r = 2*i + 2;  // right = 2*i + 2

    if (l < n && scoresArray->scoreArray[l].score < scoresArray->scoreArray[largest].score)
        largest = l;

    if (r < n && scoresArray->scoreArray[r].score < scoresArray->scoreArray[largest].score)
        largest = r;

    // If largest is not root
    if (largest != i)
    {
        scoreNode temp = scoresArray->scoreArray[i];
		scoresArray->scoreArray[i] = scoresArray->scoreArray[largest];
		scoresArray->scoreArray[largest] = temp;
		heapify(scoresArray, n, largest);
    }
}

void buildHeap(scores* scoresArray){
	for(int i = (scoresArray->actualSize) / 2; i >= 0; i--){
		heapify(scoresArray,scoresArray->actualSize,i);
	}
}

//heapsort scoresArray depending on score value
void heapSort(scores* scoresArray){
    buildHeap(scoresArray);
    for (int i=scoresArray->actualSize-1; i>=0; i--)
    {
        scoreNode temp = scoresArray->scoreArray[0];
		scoresArray->scoreArray[0] = scoresArray->scoreArray[i];
		scoresArray->scoreArray[i] = temp;
        heapify(scoresArray, i, 0);
    }
}

//inserts for each word each document's scoreNode
void calculateScoresWord(rootNode* root,char* word,generalInfo* info,scores* scoresArray,mapIndex* index){
	if(root == NULL){
		return;
	}
	
	if(root->start == NULL){
		return;
	}
	
	trieNode* tempNode = root->start->firstNode;
	for(int i=0; i < strlen(word); i++){
		while(tempNode != NULL){
			if(tempNode->character == word[i]){
				if(i != strlen(word)-1){
					tempNode = tempNode->head->firstNode;
					break;
				}else{
					//last char
					if(tempNode->postList != NULL){
						insertEachPostNode(scoresArray,tempNode,info,index,word);
					}
					return;
				}
			}else{
				tempNode = tempNode->nextNode;
			}
		}
		
		if(tempNode == NULL){
			return;
		}
	}
}

//inserts with insertionSort a scoreNode to an array of scores
void insertEachPostNode(scores* scoresArray,trieNode* node,generalInfo* info,mapIndex* index,char* word){
	postingLists* tempNode = node->postList;
	while(tempNode != NULL){
		scoreNode* insertNode = malloc(sizeof(scoreNode));
		insertNode->id = tempNode->id;
		insertNode->score = calculateScore(info,node->documentFrequency,tempNode,index);
		insertNode->words = malloc((strlen(word)+1)*sizeof(char));
		strcpy(insertNode->words,word);
		insertionSortScores(scoresArray,insertNode);
		tempNode = tempNode->next;
	}
}

//inserts a node to scores array
void insertionSortScores(scores* scoresArray, scoreNode* insertNode){
	int getPosition = binarySearchScores(scoresArray,insertNode->id);
	
	//full array
	if(scoresArray->actualSize == scoresArray->size-1){
		doubleScoresArray(scoresArray);
	}
	
	if(scoresArray->actualSize == 0){
		//empty
		copyDataScore(insertNode,&scoresArray->scoreArray[0]);
		scoresArray->actualSize++;
	}else{
		//same id
		if(insertNode->id == scoresArray->scoreArray[getPosition].id){
			scoresArray->scoreArray[getPosition].score += insertNode->score;
			scoresArray->scoreArray[getPosition].words = realloc(scoresArray->scoreArray[getPosition].words,(strlen(scoresArray->scoreArray[getPosition].words) + 2 + strlen(insertNode->words))*sizeof(char));
			strcat(scoresArray->scoreArray[getPosition].words," ");
			strcat(scoresArray->scoreArray[getPosition].words,insertNode->words);
		}else{
			//need to shift other elements to insert a new node
			int fullMoveSize = (scoresArray->actualSize - getPosition)*sizeof(scoresArray->scoreArray[scoresArray->actualSize]);
			memmove(&(scoresArray->scoreArray[getPosition+1]), &(scoresArray->scoreArray[getPosition]), fullMoveSize);
			copyDataScore(insertNode,&scoresArray->scoreArray[getPosition]);
			scoresArray->actualSize++;
		}
	}
	
	free(insertNode->words);
	insertNode->words = NULL;
	free(insertNode);
	insertNode = NULL;
}

//copy data from an old node to a new one
void copyDataScore(scoreNode* oldNode,scoreNode* newNode){
	newNode->id = oldNode->id;
	newNode->words = malloc((strlen(oldNode->words)+1)*sizeof(char));
	strcpy(newNode->words, oldNode->words);
	newNode->score = oldNode->score;
}

//binary search scores array depending on id of the document
int binarySearchScores(scores* scoresArray, int id){
	int first = 0;
	int last = scoresArray->actualSize;
	int mid = 0;
	scoreNode* array = scoresArray->scoreArray;
	while (first < last){
        int mid = first + (last-first)/2;
        if(array[mid].id == id)
            return mid;
        if (array[mid].id < id)
            first = mid + 1;
		else
            last = mid - 1;
    }
	if(last<=first){
		if(id > scoresArray->scoreArray[first].id && scoresArray->scoreArray[first].id!=-1)
			return first+1;
		else
			return first;
	}
    return mid;
}

//prints scores array for debugging
void printScoresArray(scores* scoresArray){
	for(int i=0;i<scoresArray->actualSize;i++){
		printf("%dth elem with id: %d and score: %lf and words: '%s'\n",i,scoresArray->scoreArray[i].id,scoresArray->scoreArray[i].score,scoresArray->scoreArray[i].words);
	}
}

//calculates score of a query word
double calculateScore(generalInfo* info,int docFrequency,postingLists* node,mapIndex* index){
	double idfDivision = (double) (info->totalDocuments - docFrequency + 0.5) / (docFrequency + 0.5);
	double IDF = log10(idfDivision);
	double fQi = (double) node->timesAppeared;
	double D = (double) index[node->id].words;
	double N = (double) info->totalDocuments;
	double avgdl = (double) info->totalWords / N;
	double k1 = (double) info->k1;
	double b = (double) info->b;
	double score = IDF * ((fQi*(k1+1))/(fQi + k1*(1-b + (b*(D/avgdl)))));
	return score;
}


//funcs for scores node
scores* createScoresArray(){
	scores* scoresArray = malloc(sizeof(scores));
	scoresArray->size = INIT_SIZE;
	scoresArray->actualSize = 0;
	scoresArray->scoreArray = malloc(INIT_SIZE*sizeof(scoreNode));
	for(int i=0;i<scoresArray->size;i++){
		scoresArray->scoreArray[i].id = -1;
		scoresArray->scoreArray[i].words = NULL;
		scoresArray->scoreArray[i].score = 0;
	}
	return scoresArray;
}

void doubleScoresArray(scores* scoresArray){
	int oldSize = scoresArray->size;
	scoresArray->scoreArray = realloc(scoresArray->scoreArray,2*oldSize*sizeof(scoreNode));
	scoresArray->size = 2*oldSize;
	for(int i=oldSize; i<scoresArray->size;i++){
		scoresArray->scoreArray[i].id = -1;
		scoresArray->scoreArray[i].words = NULL;
		scoresArray->scoreArray[i].score = 0;
	}
}

void destroyScoresArray(scores** scoresArray){
	for(int i=0; i < (*scoresArray)->actualSize;i++){
		free((*scoresArray)->scoreArray[i].words);
		(*scoresArray)->scoreArray[i].words = NULL;
	}
	free((*scoresArray)->scoreArray);
	(*scoresArray)->scoreArray = NULL;
	free((*scoresArray));
	(*scoresArray) = NULL;
}
