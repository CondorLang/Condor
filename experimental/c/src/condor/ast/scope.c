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
	for (int i = 0; i < scope->nodeLength; i++){
		if (scope->nodes[i].isStmt && scope->nodes[i].scopeId == scope->scopes[0]){
			printf("\n");
			ExpandASTNode(scope, &scope->nodes[i], tab);
		}
	}
}

void ExpandSubScope(Scope* scope, int id, int tab){
	for (int i = 0; i < scope->nodeLength; i++){
		if (scope->nodes[i].isStmt && scope->nodes[i].scopeId == id){
			ExpandASTNode(scope, &scope->nodes[i], tab);
		}
	}
}