#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variousMethods.h"
#include "worker.h"
#include "exit.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>

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
			//manage number of workers
			if(w >= p->noOfPaths){
				w = p->noOfPaths;
			}
			//manage number of paths for each process
			int paths = returnNumPaths(&w,p->noOfPaths);
			int tempPaths = 0;
			//create processes
			pid_t childpid;
			pid_t parent = getpid();
			indexesArray* indexesArr = NULL;
			rootNode *root = NULL;
			pid_t workers[w];
			for(int i=0;i<w;i++){
				workers[i] = 0;
			}
			
			//create fifos
			createFIFOS(w);
			sleep(1);
			for (int i=0;i<w;i++){
				childpid = fork();
				long pid;
				if (childpid == 0 ){
					
					workers[i] = getpid();
					//get specific paths
					char** pathsEach = malloc(paths*sizeof(char*));
					copyPaths(pathsEach,&p->paths[tempPaths],paths);
					
					
					indexesArr = malloc(sizeof(indexesArray));
					indexesArr->indexes = NULL;
					indexesArr->length = 0;
					//for each path
					int previousNoFiles = 0;
					for(int j=0;j<paths;j++){
						DIR *dir;
						struct dirent *direntp;
						if ((dir = opendir(pathsEach[j])) == NULL)
							fprintf(stderr, "cannot open %s\n",pathsEach[j]);
						else{
							//for each file in dir
							int noFiles = 0;
							//count files
							while((direntp=readdir(dir)) != NULL){
								if (direntp->d_name[0] != '.'){
									noFiles++;
								}
							}
							rewinddir(dir);
							char** fileNames = malloc(noFiles*sizeof(char*));
							int k=0;
							while((direntp=readdir(dir)) != NULL && k<noFiles){
								if (direntp->d_name[0] != '.'){
									fileNames[k] = malloc((strlen(direntp->d_name)+1)*sizeof(char));
									strcpy(fileNames[k],direntp->d_name);
									k++;
								}
							}
							//populate indexes
							indexesArr = populateIndexes(fileNames,pathsEach[j],noFiles,indexesArr);
							
							//free fileNames array
							for(int k=0;k<noFiles;k++){
								free(fileNames[k]);
								fileNames[k] = NULL;
							}
							free(fileNames);
							fileNames = NULL;	
							closedir(dir);
						}
					}
					
					//populate worker's trie
					root = createRoot();
					populateTrieWorker(root,indexesArr);
					
					//free paths
					for(int j=0;j<paths;j++){
						free(pathsEach[j]);
					}
					free(pathsEach);
					break;
				}
				if(childpid > 0){
					tempPaths += paths;
					if(tempPaths - paths > paths){
						continue;
					}
				}
				if(childpid < 0){
					perror("Unsuccessful fork\n");
				}
			}
			//if parent
			if(parent == getpid()){
				//open named pipes and read from stdin
				int read;
				size_t len = 0;
				char* line = NULL;
				char* l = NULL;
				while((read = getline(&line, &len, stdin)) != -1){
					l = strtok(line,"\n");
					if(l == NULL){
						continue;
					}
					parentFIFOS(w,l);
					if(strcmp(l,"/exit")==0 || strcmp(l,"\\exit")==0)
						break;
				}
				
				if(line){
					free(line);
					line = NULL;
				}
			}else{
				int pos = returnPosWorker(w,getpid(),workers);

				int logfd = createOpenLog(getpid());
				while(1){
					int ret = childFIFOS(pos,indexesArr,root,logfd);
					if(ret == 0)
						break;
				}
				
				close(logfd);
				
				deleteFIFOS(w);
			}
		}
		fclose (inFile);
	}else{
		return 0;
	}
	
	destroyPathsStruct(&p);
	
	return 0;
}