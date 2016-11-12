#include "scope.h"

void DestroyScope(Scope* scope){
	
}

void InitScope(Scope* scope){
	scope->nodeSpot = 0;
	scope->scopeLength = 0;
	scope->nodeLength = 0;
	scope->scopeSpot = 0;
}

void ExpandScope(Scope* scope, int tab){
	for (int i = 0; i < scope->scopeLength; i++){
		for (int j = 0; j < scope->nodeLength; j++){
			if (scope->nodes[i].isStmt && scope->nodes[j].scopeId == scope->scopes[i]){
				printf("\n");
				ExpandASTNode(&scope->nodes[j], tab);
			}
		}
	}
}