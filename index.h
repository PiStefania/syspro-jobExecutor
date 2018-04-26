#ifndef _INDEX_H_
#define _INDEX_H_

typedef struct mapIndex{
	char* fileName;
	char* document;			//document
	int words;				//words of each document
	int noDocs;
}mapIndex;



mapIndex* populateIndex(int lines,FILE *fp,char* fileName);
void printMapIndex(mapIndex* index,int noElems);
void destroyMapIndex(mapIndex* index,int noElems);

#endif
