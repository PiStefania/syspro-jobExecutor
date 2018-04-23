#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "variousMethods.h"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#define MAX_WORDS 10
#define STACK_SIZE 10

//specifies whether or not the arguments given are correct
void pickArgumentsMain(int argc,char* argv[],char** docfile,int* w){
	
	char* numOfWorkers = NULL;
	
	if(argc == 5){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-d")==0)
				*docfile=argv[i+1];			
			if(strcmp(argv[i],"-w")==0)
				numOfWorkers=argv[i+1];
		}
	}
	else if(argc == 3){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-d")==0)
				*docfile=argv[i+1];			
		}
	}
	else{
		printf("Wrong number of arguments. Terminate process.\n");
		exit(1);
	}
	
	if(*docfile == NULL){
		printf("Error! Wrong Arguments!\n");
		exit(1);
	}
	
	if(numOfWorkers == NULL && argc == 3){
		*w = 1;
	}else if(numOfWorkers == NULL && argc == 5){
		printf("Error getting value of w. Terminate process.\n");
		exit(1);
	}else{
		*w = atoi(numOfWorkers);
	}
	
	if(*w == 0){
		printf("Error w = 0. Terminate process.\n");
		exit(1);
	}
}

//checks file and returns array of paths
pathsStruct* checkFileGetPaths(FILE *fp){
	int read;
	size_t len = 0;
	int lines = 0;
	DIR* dirDesc;
	int pathsExist = 0;
	//change after
	char* line = NULL;
	while((read = getline(&line, &len, fp)) != -1){
		//check line whitespace
		char* temp = strtok(line,"\n");
		if(temp == NULL){
			printf("Line %d is empty. Continue.",lines);
			continue;
		}
		temp = strtok(temp," \t");
		if(temp == NULL){
			printf("Line %d is empty. Continue.",lines);
			continue;
		}
		
		//check if path exists
		dirDesc = opendir(line);
		if(dirDesc == NULL){
			printf("Path in line %d doesn't exist.\n",lines);
		}else{
			pathsExist = 1;
			lines++;
			closedir(dirDesc);
		}
	}
	if(line){
		free(line);
		line = NULL;
	}
	
	pathsStruct* p = malloc(sizeof(pathsStruct)); 
	p->paths = malloc(lines*sizeof(char*));
	p->noOfPaths = lines;
	if(pathsExist){
		rewind(fp);
		int i=0;
		while((read = getline(&line, &len, fp)) != -1){
			//check line whitespace
			char* temp = strtok(line,"\n");
			if(temp == NULL){
				continue;
			}
			temp = strtok(temp," \t");
			if(temp == NULL){
				continue;
			}

			//check if path exists
			dirDesc = opendir(line);
			if(dirDesc != NULL){
				p->paths[i] = malloc((strlen(line)+1)*sizeof(char));
				strcpy(p->paths[i],line);
				closedir(dirDesc);
				i++;
			}
		}
		
		if(line){
			free(line);
			line = NULL;
		}
	}else{
		return NULL;
	}
	return p;
}

void destroyPathsStruct(pathsStruct** p){
	for(int i=0;i<(*p)->noOfPaths;i++){
		free((*p)->paths[i]);
		(*p)->paths[i] = NULL;
	}
	free((*p)->paths);
	(*p)->paths = NULL;
	free((*p));
	*p = NULL;
}

void printPaths(pathsStruct* p){
	for(int i=0;i<p->noOfPaths;i++){
		printf("%s\n",p->paths[i]);
	}
}


//this function executes all kinds of queries (/tf, /df, /search, /exit)
/*void optionsUserInput(int K,rootNode* root,generalInfo* info,mapIndex* index){
	int read;
	size_t len = 0;
	char* line = NULL;
	char* l = NULL;
	char* token = NULL;
	//printf("Input desirable query: \n");
	while(1){
		//printf("Reminder ('/search %d words', '/df', '/df word', '/tf id word', '/exit').\n",K);
		if((read = getline(&line, &len, stdin)) != -1){
			l = strtok(line,"\n");
			if(l == NULL){
				continue;
			}
			token = strtok(l," \t");
			if(strcmp(l,"/exit")==0 || strcmp(l,"\\exit")==0){
				printf("Exiting process.\n");
				break;
			}else if(strcmp(token,"\\search")==0 || strcmp(token,"/search")==0){
				token = strtok(NULL," \t");
				if(token == NULL){
					printf("Search query with no search words.Terminating process.\n");
					break;
				}
				int words = 1;
				
				scores* scoresArray = createScoresArray(); 
				while(token!=NULL){
					//search		
					calculateScoresWord(root,token,info,scoresArray,index);
					//last word
					if(words == MAX_WORDS){
						break;
					}
					token = strtok(NULL," \t");
					words++;
				}
				
				//print k scores
				heapSort(scoresArray);
				int maxK = K;
				if(K > scoresArray->actualSize){
					maxK = scoresArray->actualSize;
				}
				printMaxKScores(scoresArray,index,maxK);
				destroyScoresArray(&scoresArray);
			}else if(strcmp(token,"/df")==0 || strcmp(token,"\\df")==0){
				token = strtok(NULL," \t");
				if(token == NULL){
					//return all
					stack* stackWord = malloc(sizeof(stack));
					initializeStack(&stackWord,STACK_SIZE);					
					DFS(root->start->firstNode,stackWord);
					destroyStack(&stackWord);
				}else{
					//return df of specific word
					while(token != NULL){
						int specificDocumentFrequency = retDocFrequency(root,token);
						if(specificDocumentFrequency == -1){
							printf("An error occured.Terminating process.\n");
							break;
						}else if(specificDocumentFrequency == -2 || specificDocumentFrequency == 0){
							printf("Not found.\n");
						}else{
							printf("%s %d\n",token,specificDocumentFrequency);
						}
						token = strtok(NULL," \t");
					}
				}
			}else if(strcmp(token,"/tf")==0 || strcmp(token,"\\tf")==0){
				token = strtok(NULL," \t");
				if(token == NULL){
					//printf("Tf query with no document id.Terminating process.\n");
					continue;
				}
				int id = atoi(token);
				if(id == 0 && token[0]!='0'){
					//printf("Tf query with false document id.Terminating process.\n");
					continue;
				}
				char* word = strtok(NULL," \t");
				if(word == NULL){
					//printf("Tf query with no search word.Terminating process.\n");
					continue;
				}
				
				int timesAppeared = returnTimesAppeared(root,id,word);
				if(timesAppeared == -1){
					printf("An error occured.Terminating process.\n");
					break;
				}else if(timesAppeared == -2 || timesAppeared == 0){
					continue;
					//printf("Not found.\n");
				}else{
					printf("%d %s %d\n",id,word,timesAppeared);
				}
				
			}else{
				printf("Your input is not a query.Terminating process.\n");				
				break;
			}
		}
		if(read == -1)
			break;
	}
	
	free(line);
	line = NULL;
}*/
