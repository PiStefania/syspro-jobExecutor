#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/errno.h>
#include "worker.h"
#include "variousMethods.h"
#include "namedPipes.h"
#include "wc.h"
#include "maxcount.h"
#include "mincount.h"

#define BUFFSIZE 1024

void serverSide(int* readfds, int* writefds,char* line,int w){	
	
	//get info from worker
	char buf[BUFFSIZE];
	int n;
	int i=0;
	char** strings = malloc(w*sizeof(char*));
	int flagOption = 0;
	while(i<w){
		//write line for worker to see
		if (write(writefds[i], line,strlen(line)) != strlen(line)) { 
			perror("Write error\n");
		}
		
		
		if((n = read(readfds[i], buf, BUFFSIZE)) > 0){
			buf[n] = '\0';
			strings[i] = malloc((strlen(buf)+1)*sizeof(char));
			strcpy(strings[i],buf);
			char* token = strtok(buf," \t");
			if(strcmp(token,"maxcount")==0){
				flagOption = 2;
			}else if(strcmp(token,"mincount")==0){
				flagOption = 3;
			}else if(strcmp(token,"wc")==0){
				flagOption = 4;
			}else if(strcmp(token,"exit")==0){
				flagOption = 5;
			}
		}else{
			strings[i] = NULL;
			perror("Error in reading line\n");
		}
		i++;
	}
	
	//print to stdout
	switch(flagOption){
		case 2:
			selectMaxCount(strings,w);
			break;
		case 3:
			selectMinCount(strings,w);
			break;
		case 4:
			sumWcs(strings,w);
			break;
		case 5:
			break;
		default:
			write(STDOUT_FILENO, "Not an option", strlen("Not an option"));
			break;
	}
	
	for(int i=0;i<w;i++){
		if(strings[i] != NULL){
			free(strings[i]);
			strings[i] = NULL;
		}
	}
	free(strings);
	strings = NULL;
	
}

int clientSide(int readfd, int writefd,indexesArray* indexesArr,rootNode* root){
	char buf[BUFFSIZE];
	int n;
	//read line from parent
	if((n = read(readfd, buf, BUFFSIZE)) > 0) {
		//write back to parent
		buf[n] = '\0';
		char* token = strtok(buf," \t");
		if(strcmp(token,"/exit")==0 || strcmp(token,"\\exit")==0){
			if (write(writefd, "exit", strlen("exit")) != strlen("exit")) { 
				exit(1);
			}
			return 0;
		}else if(strcmp(token,"\\search")==0 || strcmp(token,"/search")==0){
			if (write(STDOUT_FILENO, "\n--SEARCH--\n", n) != n) { 
				exit(1);
			}
		}else if(strcmp(token,"/maxcount")==0 || strcmp(token,"\\maxcount")==0){
			token = strtok(NULL," ");
			if(token == NULL){
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					exit(1);
				}
			}
			maxCountInfo* info = returnMaxCount(root,token);
			int lengthMax = getNumberLength(info->timesAppeared);
			char* infoStr = malloc((strlen("maxcount") + strlen(info->fileName) + lengthMax + 3)*sizeof(char));
			sprintf(infoStr,"maxcount %s %d",info->fileName,info->timesAppeared);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				destroyMaxCountInfo(info);
				free(infoStr);
				exit(1);
			}
			destroyMaxCountInfo(info);
			free(infoStr);
		}else if(strcmp(token,"/mincount")==0 || strcmp(token,"\\mincount")==0){
			token = strtok(NULL," ");
			if(token == NULL){
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					exit(1);
				}
			}
			minCountInfo* info = returnMinCount(root,token);
			int lengthMin = getNumberLength(info->timesAppeared);
			char* infoStr = malloc((strlen("mincount") + strlen(info->fileName) + lengthMin + 3)*sizeof(char));
			sprintf(infoStr,"mincount %s %d",info->fileName,info->timesAppeared);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				destroyMinCountInfo(info);
				free(infoStr);
				exit(1);
			}
			destroyMinCountInfo(info);
			free(infoStr);
		}else if(strcmp(token,"/wc")==0 || strcmp(token,"\\wc")==0){
			wcInfo* info = returnInfoStruct(indexesArr);
			int lengthLines = getNumberLength(info->lines);
			int lengthWords = getNumberLength(info->words);
			int lengthChars = getNumberLength(info->characters);
			int sum = lengthLines + lengthWords + lengthChars;
			char* infoStr = malloc((strlen("wc") + sum + 5)*sizeof(char));
			sprintf(infoStr,"wc %d %d %d",info->lines,info->words,info->characters);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				free(info);
				free(infoStr);
				exit(1);
			}
			free(info);
			free(infoStr);
		}
	}
	
	return 1;
}

void sumWcs(char** strings,int w){
	int words = 0;
	int lines = 0;
	int characters = 0;
	for(int i=0;i<w;i++){
		char* token = strtok(strings[i]," ");
		token = strtok(NULL," ");
		lines += atoi(token);
		token = strtok(NULL," ");
		words += atoi(token);
		token = strtok(NULL," ");
		characters += atoi(token);
	}
	int lengthLines = getNumberLength(lines);
	int lengthWords = getNumberLength(words);
	int lengthChars = getNumberLength(characters);
	int sum = lengthLines + lengthWords + lengthChars;
	char* infoStr = malloc((strlen("wc") + sum + 5)*sizeof(char));
	sprintf(infoStr,"wc %d %d %d\n",lines,words,characters);
	if (write(STDOUT_FILENO, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
		free(infoStr);
		exit(1);
	}
	free(infoStr);
}

void selectMaxCount(char** strings,int w){
	int maxCount = 0;
	char* fileName = NULL;
	for(int i=0;i<w;i++){
		char* token = strtok(strings[i]," ");
		token = strtok(NULL," ");
		int tempMax = atoi(strtok(NULL," "));
		if(i==0){
			fileName = token;
		}
		if(maxCount < tempMax){
			fileName = token;
			maxCount = tempMax;
		}
		else if(maxCount == tempMax){
			if(strcmp(fileName,token) > 0){
					fileName = token;
			}
		}
	}
	char* infoStr = malloc((strlen(fileName) + strlen("\n") + 1)*sizeof(char));
	sprintf(infoStr,"%s\n",fileName);
	if (write(STDOUT_FILENO, infoStr , strlen(infoStr)) != strlen(infoStr)) { 
		free(infoStr);
		exit(1);
	}
	free(infoStr);
}

void selectMinCount(char** strings,int w){
	int minCount = 1;
	char* fileName = NULL;
	for(int i=0;i<w;i++){
		char* token = strtok(strings[i]," ");
		token = strtok(NULL," ");
		int tempMin = atoi(strtok(NULL," "));
		if(i==0){
			fileName = token;
		}
		if(minCount > tempMin){
			fileName = token;
			minCount = tempMin;
		}
		else if(minCount == tempMin){
			if(strcmp(fileName,token) > 0){
				fileName = token;
			}
		}
	}
	
	char* infoStr = malloc((strlen(fileName) + 3)*sizeof(char));
	sprintf(infoStr,"%s\n",fileName);
	if (write(STDOUT_FILENO, infoStr , strlen(infoStr)) != strlen(infoStr)) { 
		free(infoStr);
		exit(1);
	}
	free(infoStr);
}


