#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "variousMethods.h"
#include "exit.h"
#define PERMS 0777

int createOpenLog(pid_t worker){
	int length = getNumberLength(worker);
	char* logFile = malloc((strlen("./log/Worker_") + length + 1)*sizeof(char));
	sprintf(logFile,"./log/Worker_%d",worker);
	int writefd;
	if((writefd = open(logFile, O_CREAT | O_WRONLY | O_APPEND, PERMS)) < 0) {
		free(logFile);
		perror("Cannot create file");
	}
	free(logFile);
	return writefd;
}

void recordQueries(int logfd,char* value){
	if(write(logfd,value,strlen(value)) != strlen(value)){
		perror("Cannot write value to file");
		return;
	}
}

void recordTime(int logfd){
	time_t t = time(NULL);
    struct tm *tm = localtime(&t);
	char* timeStr = malloc((strlen(asctime(tm))+5)*sizeof(char));
    sprintf(timeStr,"%s", asctime(tm));
	timeStr[strlen(timeStr)-1] = 0;
    if(write(logfd,timeStr,strlen(timeStr)) != strlen(timeStr)){
		free(timeStr);
		return;
	}
	free(timeStr);
}

void recordDivider(int logfd){
	if(write(logfd," : ",strlen(" : ")) != strlen(" : ")){
		return;
	}
}

void recordSearchQuery(rootNode* root,char* word,int logfd){
	trieNode* tempNode = root->start->firstNode;
	for(int i=0; i < strlen(word); i++){
		while(tempNode != NULL){
			if(tempNode->character == word[i]){
				if(i != strlen(word)-1){
					tempNode = tempNode->head->firstNode;
					break;
				}else{
					if(tempNode->postList != NULL){
						postingLists* temp = tempNode->postList->firstNode;
						int recorded = 1;
						while(temp!=NULL){
							if(temp->worker == getpid() && recorded){
								recordTime(logfd);
								recordDivider(logfd);
								recordQueries(logfd,"search");
								recordDivider(logfd);
								recordQueries(logfd,word);
								recordDivider(logfd);
								recordQueries(logfd,temp->fileName);
								recorded = 0;
							}else if(temp->worker == getpid() && recorded==0){
								recordDivider(logfd);
								recordQueries(logfd,temp->fileName);
							}
							temp = temp->next;
						}
						recordQueries(logfd,"\n");
						return;
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