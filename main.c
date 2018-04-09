#include <stdio.h>
#include <stdlib.h>
#include "index.h"
#include "invertedIndex.h"
#include "variousMethods.h"
#include "generalInfo.h"


int main (int argc,char* argv[]){
	
	int K=0;
	char* docfile = NULL;
	pickArgumentsMain(argc,argv,&docfile,&K);
	
	FILE *inFile;
	//open file for read
	inFile = fopen(docfile,"r");
	mapIndex* index = NULL;
	generalInfo* info = NULL;
	initializeGeneralInfo(&info);
	int lines = 0;
	if(inFile!=NULL)
	{
		//firstly, check for lines for format deviations
		lines = checkFileGetLines(inFile);
		if(lines == -1){
			printf("IDs set is incorrect.Terminate process.\n");
			fclose(inFile);
			destroyGeneralInfo(&info);
			exit(1);
		}else{
			//reread file and populate index
			rewind(inFile);
			index = populateIndex(lines,inFile,info);
		}
		fclose (inFile);
	}else{
		destroyGeneralInfo(&info);
		return 0;
	}
	
	rootNode *root = createRoot();
	//populate trie from index
	populateTrie(root,index,lines,info);
	
	//queries
	optionsUserInput(K,root,info,index);
	
	//delete data structures
	destroyMapIndex(index,lines);
	destroyInvertedIndex(&root);
	destroyGeneralInfo(&info);
	
	return 0;
}