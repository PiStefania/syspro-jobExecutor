#ifndef _EXIT_H_
#define _EXIT_H_

int createOpenLog(pid_t worker);
void recordQueries(int logfd,char* value);
void recordTime(int logfd);
void recordDivider(int logfd);
void recordSearchQuery(rootNode* root,char* word,int logfd);

#endif