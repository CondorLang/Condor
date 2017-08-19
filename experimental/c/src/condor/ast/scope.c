#include "scope.h"

void DestroyScope(Scope* scope){
	
}

void InitScope(Scope* scope){
	scope->nodeSpot = 0;
	scope->scopeLength = 0;
	scope->nodeLength = 0;
	scope->scopeSpot = 0;
	scope->paramsLength = 0;
	scope->paramsSpot = 0;
	scope->paramItemsLength = 0;
	scope->paramItemsSpot = 0;
}

char* ExpandScope(Scope* scope, int tab){
	char* json = "[";
	bool first = true;
	for (int i = 0; i < scope->nodeLength; i++){
		if (scope->nodes[i].isStmt && scope->nodes[i].scopeId == scope->scopes[0]){
			printf("\n");
			char* results = ExpandASTNode(scope, &scope->nodes[i], tab);
			if (first) first = false;
			else json = Concat(json, ",");
			json = Concat(json, results);
		}
	}
	return Concat(json, "]");
}

char* ExpandSubScope(Scope* scope, int id, int tab){
	char* json = "[";
	bool first = true;
	for (int i = 0; i < scope->nodeLength; i++){
		if (scope->nodes[i].isStmt && scope->nodes[i].scopeId == id){
			char* results = ExpandASTNode(scope, &scope->nodes[i], tab + 2);
			if (first) first = false;
			else json = Concat(json, ",");
			json = Concat(json, results);
		}
	}
	return Concat(json, "]");
}