#ifndef _INDEX_H_
#define _INDEX_H_

typedef struct mapIndex{
	char** documents;			//document
	char* fileName;
	int words;
	int noDocs;
	int characters;
}mapIndex;



mapIndex* populateIndex(int lines,FILE *fp,char* fileName);
void printMapIndex(mapIndex* index,int noElems);
void destroyMapIndex(mapIndex* index,int noElems);

#endif
