#ifndef _NAMEDPIPES_H_
#define _NAMEDPIPES_H_

#include "worker.h"
#include "exit.h"

void serverSide(int* readfds, int* writefd,char* line,int w);
int clientSide(int readfd, int writefd,indexesArray* indexesArr,rootNode* root,int logfd);

//for option handling
void sumWcs(char** strings,int w);
void selectMaxCount(char** strings,int w);
void selectMinCount(char** strings,int w);
void destroySearchWords(char** searchWords,int words);

#endif