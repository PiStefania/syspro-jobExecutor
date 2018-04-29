#ifndef _NAMEDPIPES_H_
#define _NAMEDPIPES_H_

#include "worker.h"

void serverSide(int* readfds, int* writefd,char* line,int w);
int clientSide(int readfd, int writefd,indexesArray* indexesArr,rootNode* root);

//for option handling
void sumWcs(char** strings,int w);

#endif