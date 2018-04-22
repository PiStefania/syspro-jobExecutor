#include <stdio.h>
#include <stdlib.h>
#include "index.h"
#include "invertedIndex.h"
#include "variousMethods.h"
#include "generalInfo.h"


int main (int argc,char* argv[]){
	
	int w=0;
	char* docfile = NULL;
	pickArgumentsMain(argc,argv,&docfile,&w);
	
	FILE *inFile;
	//open file for read
	inFile = fopen(docfile,"r");
	mapIndex* index = NULL;
	int lines = 0;
	if(inFile!=NULL)
	{
		//firstly, check for lines for format deviations
		lines = checkFileGetLines(inFile);
		if(lines == -1){
			printf("Paths in file do not exist.\n");
			fclose(inFile);
			exit(1);
		}else{
			//reread file and populate index
			rewind(inFile);
		}
		fclose (inFile);
	}else{
		return 0;
	}
	
	return 0;
}