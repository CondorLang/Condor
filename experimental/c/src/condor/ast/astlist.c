#include "astlist.h"

void InitParams(ASTList* params, int totalFuncs){
	for (int i = 0; i < totalFuncs; i++){
		params[i].first = NULL;
		params[i].last = NULL;
		params[i].current = NULL;
		params[i].size = -1;
		params[i].nodeId = -1;
	}
}

void InitParamItems(ASTListItem* paramItems, int totalParamItems){
	for (int i = 0; i < totalParamItems; i++){
		paramItems[i].node = NULL;
		paramItems[i].prev = NULL;
		paramItems[i].next = NULL;
	}
}