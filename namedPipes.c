#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/errno.h>

#define BUFFSIZE 1024

void serverSide(int readfd, int writefd,char* line){	
	if (write(writefd, line,strlen(line)) != strlen(line)) { 
		perror("Write error\n");
	}
}

int clientSide(int readfd, int writefd){
	char buf[BUFFSIZE];
	int n;
	if((n = read(readfd, buf, BUFFSIZE)) > 0) {
		buf[n] = '\0';
		char* token = strtok(buf," \t");
		if(strcmp(token,"/exit")==0 || strcmp(token,"\\exit")==0){
			write(STDOUT_FILENO, "\n--EXIT--\n", strlen("\n--EXIT--\n"));
			return 0;
		}else if(strcmp(token,"\\search")==0 || strcmp(token,"/search")==0){
			if (write(STDOUT_FILENO, "\n--SEARCH--\n", n) != n) { 
				exit(1);
			}
		}else if(strcmp(token,"/maxcount")==0 || strcmp(token,"\\maxcount")==0){
			if (write(STDOUT_FILENO, "\n--MAXCOUNT--\n", n) != n) { 
				exit(1);
			}
		}else if(strcmp(token,"/mincount")==0 || strcmp(token,"\\mincount")==0){
			/*token = strtok(NULL," \t");
			if(token == NULL){
				//printf("Tf query with no document id.Terminating process.\n");
				continue;
			}*/
			if (write(STDOUT_FILENO, "\n--MINCOUNT--\n", n) != n) { 
				exit(1);
			}
		}else if(strcmp(token,"/wc")==0 || strcmp(token,"\\wc")==0){
			if (write(STDOUT_FILENO, "\n--WC--\n", n) != n) { 
				exit(1);
			}
		}
	}
	
	return 1;
}
