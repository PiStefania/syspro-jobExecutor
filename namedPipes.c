#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/errno.h>
#include <time.h>
#include "worker.h"
#include "variousMethods.h"
#include "namedPipes.h"
#include "wc.h"
#include "maxcount.h"
#include "mincount.h"
#include "exit.h"
#include "search.h"

#define BUFFSIZE 10000
#define PERMS 0777


void serverSide(int* readfds, int* writefds,char* line,int w){	
	
	//get info from worker
	char buf[BUFFSIZE];
	int n;
	int i=0;
	char** strings = malloc(w*sizeof(char*));
	int flagOption = 0;
	
	//start clock
	clock_t start_t = clock();
	int noWorkers = w;
	
	while(i<w){
		//write line for worker to see
		if (write(writefds[i], line, strlen(line)) != strlen(line)) { 
			perror("Write error\n");
		}
		
		if((n = read(readfds[i], buf, BUFFSIZE)) > 0){
			buf[n] = '\0';
			strings[i] = malloc((strlen(buf)+1)*sizeof(char));
			strcpy(strings[i],buf);
			char* token = strtok(buf,"|");
			double deadline;
			clock_t difference;
			double secs; 
			if(strcmp(token,"search")!=0){
				deadline = 0;
				secs = 0;
				difference = 0;
			}else{
				char* deadl = strtok(NULL," ");
				deadline = atof(deadl);
				difference = clock() - start_t;
				secs = (double) difference / CLOCKS_PER_SEC;
			}	
			if(strcmp(token,"search")==0){
				flagOption = 1;
				if(secs < deadline){
					char* remaining = strtok(NULL,"");
					write(STDOUT_FILENO, remaining, strlen(remaining));		
				}else{
					noWorkers--;
				}
			}
			else if(strcmp(token,"maxcount")==0){
				flagOption = 2;
			}else if(strcmp(token,"mincount")==0){
				flagOption = 3;
			}else if(strcmp(token,"wc")==0){
				flagOption = 4;
			}else if(strcmp(token,"exit")==0){
				flagOption = 5;
			}
		}
		i++;
	}
	
	//print to stdout
	char* infoStr = NULL;
	int writefd;
	int lengthW;
	int lengthAnswered;
	switch(flagOption){
		case 1:
			lengthW = getNumberLength(w);
			lengthAnswered = getNumberLength(noWorkers);
			char* workersAnswer = malloc((strlen("Workers answered") + 5 + lengthW + lengthAnswered)*sizeof(char));
			sprintf(workersAnswer,"%d/%d Workers answered\n",noWorkers,w);
			write(STDOUT_FILENO, workersAnswer , strlen(workersAnswer));
			free(workersAnswer);
			workersAnswer = NULL;
			break;
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
			write(STDOUT_FILENO, "Not found\n", strlen("Not found\n"));
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

int clientSide(int readfd, int writefd,indexesArray* indexesArr,rootNode* root,int logfd){
	char buf[BUFFSIZE];
	int n;
	//read line from parent
	if((n = read(readfd, buf, BUFFSIZE)) > 0) {
		//write back to parent
		buf[n] = '\0';
		char* token = strtok(buf," \t");
		if(strcmp(token,"\\search")==0 || strcmp(token,"/search")==0){
			char* remaining = strtok(NULL,"\n");
			
			if(token == NULL){
				if (write(writefd, "error", strlen("error")) != strlen("error")) { 
					return 1;
				}
			}

			char* tempStr = malloc((strlen(remaining)+1)*sizeof(char));
			char* temptStrPointer = tempStr;
			strcpy(tempStr,remaining);
			int words = 0;
			token = strtok(remaining," ");
			while(token!=NULL){
				words++;
				token = strtok(NULL," ");
			}
			
			if(words-2 < 0){
				if (write(writefd, "error", strlen("error")) != strlen("error")) { 
					free(tempStr);
					return 1;
				}
				free(tempStr);
				return 1;
			}
			int i=0;
			char** searchWords = malloc((words-2)*sizeof(char*));
			
			tempStr = strtok(tempStr," ");
			int deadline = 0;
			while(i<words && tempStr!=NULL){
				//search
				if(i==words-2){
					if(strcmp(tempStr,"-d")!=0){
						if (write(writefd, "error", strlen("error")) != strlen("error")) { 
							destroySearchWords(searchWords,words-2);
							free(temptStrPointer);
							temptStrPointer = NULL;
							return 1;
						}
						free(temptStrPointer);
						temptStrPointer = NULL;
						destroySearchWords(searchWords,words-2);
						return 1;
					}
				}
				else if(i==words-1){
					deadline = atoi(tempStr);
					if(deadline == 0){
						if (write(writefd, "error", strlen("error")) != strlen("error")) {
							free(temptStrPointer);
							temptStrPointer = NULL;
							destroySearchWords(searchWords,words-2);
							return 1;
						}
						free(temptStrPointer);
						temptStrPointer = NULL;
						destroySearchWords(searchWords,words-2);
						return 1;
					}
				}else{
					searchWords[i] = malloc((strlen(tempStr)+1)*sizeof(char));
					strcpy(searchWords[i],tempStr);
					recordSearchQuery(root,searchWords[i],logfd);
				}
				tempStr = strtok(NULL," ");
				i++;
			}
						
			free(temptStrPointer);
			temptStrPointer = NULL;
			searchInfo* arrayInfo = searchFiles(root,searchWords,words-2);
			if(arrayInfo != NULL){
				foundLines* array = combinedLines(arrayInfo,indexesArr);
				char* allLines = searchQueries(array,deadline);
				if(write(writefd, allLines, strlen(allLines)) != strlen(allLines)){
					destroySearchWords(searchWords,words-2);
					free(allLines);
					destroyFoundLines(array);
					destroyArrayInfo(arrayInfo);
					return 1;
				}
				free(allLines);
				destroyFoundLines(array);
				destroyArrayInfo(arrayInfo);
			}else{
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					destroySearchWords(searchWords,words-2);
					return 1;
				}
				destroySearchWords(searchWords,words-2);
				return 1;
			}
			destroySearchWords(searchWords,words-2);
		}else if(strcmp(token,"/maxcount")==0 || strcmp(token,"\\maxcount")==0){
			token = strtok(NULL," ");
			if(token == NULL){
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					return 1;
				}
			}
			
			maxCountInfo* info = returnMaxCount(root,token);
			if(info == NULL){
				if (write(writefd,"error", strlen("error")) != strlen("error")) {
					return 1;
				}
				return 1;
			}
			
			//record in log file
			recordTime(logfd);
			recordDivider(logfd);
			recordQueries(logfd,"maxcount");
			recordDivider(logfd);
			recordQueries(logfd,token);
			recordDivider(logfd);
			recordQueries(logfd,info->fileName);
			recordQueries(logfd,"\n");
			
			int lengthMax = getNumberLength(info->timesAppeared);
			char* infoStr = malloc((strlen("maxcount") + strlen(info->fileName) + lengthMax + 4)*sizeof(char));
			sprintf(infoStr,"maxcount| %s %d",info->fileName,info->timesAppeared);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				destroyMaxCountInfo(info);
				free(infoStr);
				return 1;
			}
			destroyMaxCountInfo(info);
			free(infoStr);
		}else if(strcmp(token,"/mincount")==0 || strcmp(token,"\\mincount")==0){
			token = strtok(NULL," ");
			if(token == NULL){
				if(write(writefd, "error", strlen("error")) != strlen("error")){
					return 1;
				}
			}
			
			
			minCountInfo* info = returnMinCount(root,token);
			if(info == NULL){
				if (write(writefd,"error", strlen("error")) != strlen("error")) {
					return 1;
				}
				return 1;
			}
			
			//record in log file
			recordTime(logfd);
			recordDivider(logfd);
			recordQueries(logfd,"mincount");
			recordDivider(logfd);
			recordQueries(logfd,token);
			recordDivider(logfd);
			recordQueries(logfd,info->fileName);
			recordQueries(logfd,"\n");
			
			int lengthMin = getNumberLength(info->timesAppeared);
			char* infoStr = malloc((strlen("mincount") + strlen(info->fileName) + lengthMin + 4)*sizeof(char));
			sprintf(infoStr,"mincount| %s %d",info->fileName,info->timesAppeared);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				destroyMinCountInfo(info);
				free(infoStr);
				return 1;
			}
			destroyMinCountInfo(info);
			free(infoStr);
		}else if(strcmp(token,"/wc")==0 || strcmp(token,"\\wc")==0){
			//record in log file
			recordTime(logfd);
			recordDivider(logfd);
			recordQueries(logfd,"wc");
			recordDivider(logfd);
			
			
			wcInfo* info = returnInfoStruct(indexesArr);
			int lengthLines = getNumberLength(info->lines);
			int lengthWords = getNumberLength(info->words);
			int lengthChars = getNumberLength(info->characters);
			int sum = lengthLines + lengthWords + lengthChars;
			char* infoStr = malloc((strlen("wc") + sum + 6)*sizeof(char));
			sprintf(infoStr,"wc|%d %d %d",info->lines,info->words,info->characters);
			char* logStr = malloc((sum+9)*sizeof(char));
			sprintf(logStr,"%d : %d : %d\n",info->lines,info->words,info->characters);
			//record in log file
			recordQueries(logfd,logStr);
			if (write(writefd, infoStr, strlen(infoStr)) != strlen(infoStr)) { 
				free(info);
				free(infoStr);
				free(logStr);
				return 1;
			}
			free(info);
			free(infoStr);
			free(logStr);
		}else if(strcmp(token,"/exit")==0 || strcmp(token,"\\exit")==0){
			if (write(writefd, "exit", strlen("exit")) != strlen("exit")) { 
				return 1;
			}
			
			//free indexes
			destroyIndexes(indexesArr);

			//free inverted index
			destroyInvertedIndex(&root);
			return 0;
		}else{
			if (write(writefd,"error", strlen("error")) != strlen("error")) {
				return 1;
			}
			return 1;
		}
	}
	
	return 1;
}

//sum wcs of each worker
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
		return;
	}
	free(infoStr);
}

//select max count of word from workers
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
		return;
	}
	free(infoStr);
}

//select min count of word from workers
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
		return;
	}
	
	free(infoStr);
}

//destroy searchWords, array of strings for search query
void destroySearchWords(char** searchWords,int words){
	for(int i=0;i<words;i++){
		if(searchWords[i]!=NULL){
			free(searchWords[i]);
			searchWords[i] = NULL;
		}
	}
	free(searchWords);
	searchWords = NULL;
}
