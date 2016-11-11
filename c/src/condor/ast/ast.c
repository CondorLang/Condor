#include "ast.h"

void InitVars(ASTVar vars[], int len){
	for (int i = 0; i < len; i++){
		vars[i].meta.id = i;
		vars[i].name = NULL;
		vars[i].value = NULL;
	}
}

void InitBinaryExprs(ASTBinaryExpr exprs[], int len){
	for (int i = 0; i < len; i++){
		exprs[i].meta.id = i;
		exprs[i].op = UNDEFINED;
	}
}

void InitLiterals(ASTLiteral lits[], int len){
	for (int i = 0; i < len; i++){
		lits[i].meta.id = i;
		lits[i].dataType = UNDEFINED;
		lits[i].value = NULL;
	}
}