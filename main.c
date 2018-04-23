#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variousMethods.h"
#include "worker.h"

#include <unistd.h>
#include <sys/types.h>

int main (int argc,char* argv[]){
	
	int w=0;
	char* docfile = NULL;
	pickArgumentsMain(argc,argv,&docfile,&w);
	
	FILE *inFile;
	//open file for read
	inFile = fopen(docfile,"r");
	pathsStruct* p = NULL;
	if(inFile!=NULL)
	{
		//firstly, check for lines for format deviations
		p = checkFileGetPaths(inFile);
		if(p == NULL){
			printf("Paths in file do not exist.\n");
			fclose(inFile);
			destroyPathsStruct(&p);
			exit(1);
		}else{
			//printf("Paths created\n");
			//printPaths(p);
			//manage number of workers
			if(w >= p->noOfPaths){
				w = p->noOfPaths;
			}
			//manage number of paths for each process
			//printf("W: %d, paths: %d\n",w,p->noOfPaths);
			int paths = returnNumPaths(w,p->noOfPaths);
			//printf("Paths for each worker: %d\n",paths);
			//create processes
			pid_t childpid;
			for (int i=0;i<w;i++){
				if ((childpid = fork()) <= 0 ){
					//populate worker's index and trie and then wait
					//get specific paths
					char** pathsEach = malloc(paths*sizeof(char*));
					memmove(pathsEach[0],p->paths[i],paths*sizeof(char*));
					free(pathsEach);
					break;	//wait meta
				}
			}
		}
		fclose (inFile);
	}else{
		return 0;
	}
	
	destroyPathsStruct(&p);
	
	return 0;
}