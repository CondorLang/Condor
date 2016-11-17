#include "ast.h"

int ASTNODE_ID_SPOT = 1;

void InitNodes(ASTNode nodes[], int len){
	for (int i = 0; i < len; i++){
		nodes[i].id = ASTNODE_ID_SPOT++;
		nodes[i].type = UNDEFINED;
		nodes[i].isStmt = false;
		nodes[i].scopeId = -1;
	}
}

void DestroyNodes(ASTNode nodes[], int len){
	for (int i = 0; i < len; i++){
		int type = (int) nodes[i].type;
		switch (type){
			case VAR: Free(nodes[i].meta.varExpr.name); break;
		}
	}
}

void ExpandASTNode(Scope* scope, ASTNode* node, int tab){
	if (node == NULL) return;
	char tabs[(tab * 2) + 1];
	for (int i = 0; i < tab * 2; i++) tabs[i] = ' ';
	tabs[(tab * 2)] = '\0';

	printf("%s[%s]: \n %s-id: %d\n", tabs, TokenToString(node->type), tabs, node->id);

	int type = (int) node->type;
	switch (type){
		case VAR: {
			printf(" %s-name: %s\n %s-inc: %s\n %s-value\n", tabs, node->meta.varExpr.name, tabs, TokenToString(node->meta.varExpr.inc), tabs);
			ExpandASTNode(scope, node->meta.varExpr.value, tab + 2);
			break;	
		}
		case STRING: {
			printf(" %s-value: \"%s\"\n", tabs, node->meta.stringExpr.value);
			break;
		}
		case BINARY: {
			printf(" %s-operator: %s\n %s-left:\n", tabs, TokenToString(node->meta.binaryExpr.op), tabs);
			ExpandASTNode(scope, node->meta.binaryExpr.left, tab + 2);
			printf(" %s-right:\n", tabs);
			ExpandASTNode(scope, node->meta.binaryExpr.right, tab + 2);
			break;
		}
		case BOOLEAN: {
			printf(" %s-value: %d\n", tabs, node->meta.booleanExpr.value);
			break;	
		}
		case BYTE: {
			printf(" %s-value: %d\n", tabs, node->meta.byteExpr.value);
			break;	
		}
		case SHORT: {
			printf(" %s-value: %d\n", tabs, node->meta.shortExpr.value);
			break;	
		}
		case INT: {
			printf(" %s-value: %d\n", tabs, node->meta.intExpr.value);
			break;	
		}
		case FLOAT: {
			printf(" %s-value: %f\n", tabs, node->meta.floatExpr.value);
			break;	
		}
		case DOUBLE: {
			printf(" %s-value: %f\n", tabs, node->meta.doubleExpr.value);
			break;	
		}
		case LONG: {
			printf(" %s-value: %ld\n", tabs, node->meta.longExpr.value);
			break;	
		}
		case FOR: {
			printf(" %s-var:\n", tabs);
			ExpandASTNode(scope, node->meta.forExpr.var, tab + 2);
			printf(" %s-condition:\n", tabs);
			ExpandASTNode(scope, node->meta.forExpr.condition, tab + 2);
			printf(" %s-incrementor:\n", tabs);
			ExpandASTNode(scope, node->meta.forExpr.inc, tab + 2);
			printf(" %s-body:\n", tabs);
			ExpandSubScope(scope, node->meta.forExpr.body, tab + 2);
			break;
		}
		case WHILE: case IF: {
			printf(" %s-condition:\n", tabs);
			ExpandASTNode(scope, node->meta.ifExpr.condition, tab + 2);
			printf(" %s-body:\n", tabs);
			ExpandSubScope(scope, node->meta.ifExpr.body, tab + 2);
		}
	}
}

ASTNode* FindSymbol(Scope* scope, char* name){
	for (int i = 0; i < scope->nodeLength; i++){
		Token t = scope->nodes[i].type;
		if (t == OBJECT) {
			NOT_IMPLEMENTED("Symbol for OBJECT");
		}
		else if (t == FUNC){
			NOT_IMPLEMENTED("Symbol for FUNC");
		}
		else if (t == VAR && strcmp(name, scope->nodes[i].meta.varExpr.name) == 0){
			return &scope->nodes[i];
		}
	}
	return NULL;
}