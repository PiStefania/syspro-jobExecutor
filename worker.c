#include <stdio.h>
#include "worker.h"

int returnNumPaths(int w,int noPaths){
	int numPaths = 0;
	if(w == noPaths){
		numPaths = 1;
	}else{
		numPaths = noPaths / w;
	}
	return numPaths;
}
