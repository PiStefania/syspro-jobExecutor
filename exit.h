#ifndef _EXIT_H_
#define _EXIT_H_

typedef struct fileInfo{
	char* logFile;
	int logfd;
}fileInfo;

fileInfo* createOpenLog(pid_t worker);
void recordQueries(int logfd,char* value);
void recordTime(int logfd);
void recordDivider(int logfd);

#endif