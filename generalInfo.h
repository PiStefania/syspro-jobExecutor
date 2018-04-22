#ifndef _GENERALINFO_H_
#define _GENERALINFO_H_

typedef struct generalInfo{
	int totalWords;				//total words of all documents
	int totalDocuments;			//total documents
	double k1;					//for score
	double b;					//for score
}generalInfo;


void initializeGeneralInfo(generalInfo** info);
void destroyGeneralInfo(generalInfo** info);

#endif