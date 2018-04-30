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
#include "exit.h"
#include "search.h"

#define BUFFSIZE 1024
#define PERMS 0777


void serverSide(int* readfds, int* writefds,char* line,int w){	
	
	//get info from worker
	char buf[BUFFSIZE];
	int n;
	int i=0;
	char** strings = malloc(w*sizeof(char*));
	int flagOption = 0;
	char** logfiles = malloc(w*sizeof(char*));
	while(i<w){
		//write line for worker to see
		if (write(writefds[i], line,strlen(line)) != strlen(line)) { 
			perror("Write error\n");
		}
		
		if((n = read(readfds[i], buf, BUFFSIZE)) > 0){
			buf[n] = '\0';
			strings[i] = malloc((strlen(buf)+1)*sizeof(char));
			strcpy(strings[i],buf);
			char* token = strtok(buf,"|");
			if(strcmp(token,"search")==0){
				logfiles[i] = malloc((strlen("./log/Worker_") + 6) * sizeof(char));
				strcpy(logfiles[i],strtok(NULL," "));
				flagOption = 1;
			}else if(strcmp(token,"maxcount")==0){
				logfiles[i] = malloc((strlen("./log/Worker_") + 6) * sizeof(char));
				strcpy(logfiles[i],strtok(NULL," "));
				flagOption = 2;
			}else if(strcmp(token,"mincount")==0){
				logfiles[i] = malloc((strlen("./log/Worker_") + 6) * sizeof(char));
				strcpy(logfiles[i],strtok(NULL," "));
				flagOption = 3;
			}else if(strcmp(token,"wc")==0){
				logfiles[i] = NULL;
				flagOption = 4;
			}else if(strcmp(token,"exit")==0){
				logfiles[i] = NULL;
				flagOption = 5;
			}
		}else{
			strings[i] = NULL;
			logfiles[i] = NULL;
			perror("Error in reading line\n");
		}
		i++;
	}
	
	//print to stdout
	char* infoStr = NULL;
	int writefd;
	switch(flagOption){
		case 1:
			//printSearchLines();
			break;
		case 2:
			infoStr = selectMaxCount(strings,w);
			for(int i=0;i<w;i++){
				//record in log file
				if((writefd = open(logfiles[i], O_CREAT | O_WRONLY | O_APPEND, PERMS)) < 0) {
					perror("Cannot open file");
				}
				recordQueries(writefd,infoStr);
				recordQueries(writefd,"\n");
			}	
			free(infoStr);
			break;
		case 3:
			infoStr = selectMinCount(strings,w);
			for(int i=0;i<w;i++){
				//record in log file
				if((writefd = open(logfiles[i], O_CREAT | O_WRONLY | O_APPEND, PERMS)) < 0) {
					perror("Cannot open file");
				}
				recordQueries(writefd,infoStr);
				recordQueries(writefd,"\n");
			}	
			free(infoStr);
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
		if(logfiles[i]!=NULL){
			free(logfiles[i]);
			logfiles[i] = NULL;
		}
		if(strings[i] != NULL){
			free(strings[i]);
			strings[i] = NULL;
		}
	}
	free(logfiles);
	logfiles = NULL;
	free(strings);
	strings = NULL;
	
}

int clientSide(int readfd, int writefd,indexesArray* indexesArr,rootNode* root,fileInfo* infoFile){
	char buf[BUFFSIZE];
	int n;
	//read line from parent
	if((n = read(readfd, buf, BUFFSIZE)) > 0) {
		//write back to parent
		buf[n] = '\0';
		char* token = strtok(buf," \t");
		if(strcmp(token,"\\search")==0 || strcmp(token,"/search")==0){
			//record in log file
			recordTime(infoFile->logfd);
			recordDivider(infoFile->logfd);
			recordQueries(infoFile->logfd,"search");
			recordDivider(infoFile->logfd);
			
			char* remaining = strtok(NULL,"\n");
			if(token == NULL){
				if (write(writefd, "error", strlen("error")) != strlen("error")) { 
					exit(1);
				}
			}

			char* tempStr = malloc((strlen(remaining)+1)*sizeof(char));
			strcpy(tempStr,remaining);
			int words = 0;
			token = strtok(remaining," ");
			while(token!=NULL){
				words++;
				token = strtok(NULL," ");
			}
			
			//record in log file
			recordQueries(infoFile->logfd,tempStr);
			recordDivider(infoFile->logfd);
			
			int i=0;
			char** searchWords = malloc((words-2)*sizeof(char*));
			
			while(i<words && tempStr!=NULL){
				//search
				tempStr = strtok(NULL," ");
				if(i==words-2){
					if(strcmp(tempStr,"-d")!=0){
						if (write(writefd, "error", strlen("error")) != strlen("error")) { 
							exit(1);
						}
					}
				}
				else if(i==words-1){
					if(atoi(tempStr) == 0){
						if (write(writefd, "error", strlen("error")) != strlen("error")) { 
							exit(1);
						}
					}
				}else{
					searchWords[i] = malloc((strlen(tempStr)+1)*sizeof(char));
					strcpy(searchWords[i],tempStr);
				}
				i++;
			}
			
			char* searchQuery = searchFiles(root,searchWords,words-2);
			
		}else if(strcmp(token,"/maxcount")==0 || strcmp(token,"\\maxcount")==0){
			//record in log file
			recordTime(infoFile->logfd);
			recordDivider(infoFile->logfd);
			recordQueries(infoFile->logfd,"maxcount");
			recordDivider(infoFile->logfd);
			
			token = strtok(NULL," ");
			if(token == NULL){
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					exit(1);
				}
			}
			
			//record in log file
			recordQueries(infoFile->logfd,token);
			recordDivider(infoFile->logfd);
			
			maxCountInfo* info = returnMaxCount(root,token);
			int lengthMax = getNumberLength(info->timesAppeared);
			char* infoStr = malloc((strlen("maxcount") + strlen(info->fileName) + lengthMax + 4 + strlen(infoFile->logFile))*sizeof(char));
			sprintf(infoStr,"maxcount|%s %s %d",infoFile->logFile,info->fileName,info->timesAppeared);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				destroyMaxCountInfo(info);
				free(infoStr);
				exit(1);
			}
			destroyMaxCountInfo(info);
			free(infoStr);
		}else if(strcmp(token,"/mincount")==0 || strcmp(token,"\\mincount")==0){
			//record in log file
			recordTime(infoFile->logfd);
			recordDivider(infoFile->logfd);
			recordQueries(infoFile->logfd,"mincount");
			recordDivider(infoFile->logfd);
			
			token = strtok(NULL," ");
			if(token == NULL){
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					exit(1);
				}
			}
			
			//record in log file
			recordQueries(infoFile->logfd,token);
			recordDivider(infoFile->logfd);
			
			minCountInfo* info = returnMinCount(root,token);
			int lengthMin = getNumberLength(info->timesAppeared);
			char* infoStr = malloc((strlen("mincount") + strlen(info->fileName) + lengthMin + 4 + strlen(infoFile->logFile))*sizeof(char));
			sprintf(infoStr,"mincount|%s %s %d",infoFile->logFile,info->fileName,info->timesAppeared);
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
			char* infoStr = malloc((strlen("wc") + sum + 6)*sizeof(char));
			sprintf(infoStr,"wc|%d %d %d",info->lines,info->words,info->characters);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				free(info);
				free(infoStr);
				exit(1);
			}
			free(info);
			free(infoStr);
		}else if(strcmp(token,"/exit")==0 || strcmp(token,"\\exit")==0){
			if (write(writefd, "exit", strlen("exit")) != strlen("exit")) { 
				exit(1);
			}
			
			//free indexes
			destroyIndexes(indexesArr);

			//free inverted index
			destroyInvertedIndex(&root);
			return 0;
		}
	}
	
	return 1;
}

void sumWcs(char** strings,int w){
	int words = 0;
	int lines = 0;
	int characters = 0;
	for(int i=0;i<w;i++){
		char* token = strtok(strings[i],"|");
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

char* selectMaxCount(char** strings,int w){
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
	return infoStr;
}

char* selectMinCount(char** strings,int w){
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
	return infoStr;
}


