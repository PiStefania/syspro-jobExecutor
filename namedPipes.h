#ifndef _NAMEDPIPES_H_
#define _NAMEDPIPES_H_

#include "worker.h"
#include "exit.h"

void serverSide(int* readfds, int* writefd,char* line,int w);
int clientSide(int readfd, int writefd,indexesArray* indexesArr,rootNode* root,fileInfo* infoFile);

//for option handling
void sumWcs(char** strings,int w);
char* selectMaxCount(char** strings,int w);
char* selectMinCount(char** strings,int w);

#endif