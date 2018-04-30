#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "variousMethods.h"
#include "exit.h"
#define PERMS 0777


fileInfo* createOpenLog(pid_t worker){
	fileInfo* info = malloc(sizeof(fileInfo));
	int length = getNumberLength(worker);
	info->logFile = malloc((strlen("./log/Worker_") + length + 1)*sizeof(char));
	sprintf(info->logFile,"./log/Worker_%d",worker);
	int writefd;
	if((writefd = open(info->logFile, O_CREAT | O_WRONLY | O_APPEND, PERMS)) < 0) {
		perror("Cannot create file");
	}
	info->logfd = writefd;
	return info;
}

void recordQueries(int logfd,char* value){
	if(write(logfd,value,strlen(value)) != strlen(value)){
		perror("Cannot write value to file");
		exit(1);
	}
}

void recordTime(int logfd){
	time_t t = time(NULL);
    struct tm *tm = localtime(&t);
	char* timeStr = malloc((strlen(asctime(tm))+5)*sizeof(char));
    sprintf(timeStr,"%s  : ", asctime(tm));
	timeStr[strlen(timeStr)-4] = 0;
    if(write(logfd,timeStr,strlen(timeStr)) != strlen(timeStr)){
		exit(1);
	}
	free(timeStr);
}

void recordDivider(int logfd){
	if(write(logfd," : ",strlen(" : ")) != strlen(" : ")){
		exit(1);
	}
}