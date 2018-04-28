#ifndef _NAMEDPIPES_H_
#define _NAMEDPIPES_H_

void serverSide(int readfd, int writefd,char* line);
int clientSide(int readfd, int writefd);

#endif