#ifndef _INDEX_H_
#define _INDEX_H_

typedef struct mapIndex{
	char** documents;			//documents
	char* fileName;				//fileName of which it was created
	int words;					//no words
	int noDocs;					//no lines
	int characters;				//no of characters
}mapIndex;



mapIndex* populateIndex(int lines,FILE *fp,char* fileName);
void printMapIndex(mapIndex* index,int noElems);
void destroyMapIndex(mapIndex* index,int noElems);

#endif
