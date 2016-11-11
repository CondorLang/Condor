#include "scope.h"

void DestroyScope(Scope* scope){
	for (int i = 0; i < scope->audit->var; i++){
		if (scope->vars[i].name != NULL) Free(scope->vars[i].name);
	}
	for (int i = 0; i < scope->audit->lit; i++){
		if (scope->lits[i].value != NULL) Free(scope->lits[i].value);
	}
}

void InitScope(Scope* scope){
	scope->vars = NULL;
	scope->binarys = NULL;
	scope->varSpot = 0;
	scope->binarySpot = 0;
	scope->audit = NULL;
}