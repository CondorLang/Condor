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
			case STRING: Free(nodes[i].meta.stringExpr.value); break;
		}
	}
}

char* ExpandASTNode(Scope* scope, ASTNode* node, int tab){
	if (node == NULL) return "";
	char tabs[(tab * 2) + 1];
	for (int i = 0; i < tab * 2; i++) tabs[i] = ' ';
	tabs[(tab * 2)] = '\0';

	char* json = "";
	char tmp1[80];
	sprintf(tmp1, "{\"type\": \"%s\", \"id\": %d", TokenToString(node->type), node->id);
	json = Concat(json, tmp1);

			#ifdef EXPAND_AST
	printf("	%s[%s]: \n %s-id: %d\n", tabs, TokenToString(node->type), tabs, node->id);
#endif

	int type = (int) node->type;
	switch (type){
		case VAR: {
			#ifdef EXPAND_AST
				printf(" %s-name: %s\n %s-inc: %s\n %s-dataType: %s\n %s-value\n", tabs, node->meta.varExpr.name, tabs, TokenToString(node->meta.varExpr.inc), tabs, TokenToString(node->meta.varExpr.dataType), tabs);
			#endif
			char* value = ExpandASTNode(scope, node->meta.varExpr.value, tab + 2);
			char temp2[500];
			bool nothing = strlen(value) == 0;
			sprintf(temp2, ",\"name\": \"%s\", \"inc\": \"%s\", \"dataType\": \"%s\", \"value\": %s", node->meta.varExpr.name, TokenToString(node->meta.varExpr.inc), TokenToString(node->meta.varExpr.dataType), nothing ? "\"" : " ");
			char* result = Concat(temp2, value);
			if (nothing) result = Concat(result, "\"");
			json = Concat(json, result);
			break;	
		}
		case RETURN: {
			#ifdef EXPAND_AST
				printf(" %s-value\n", tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.returnStmt.value, tab + 2);
			json = Concat(json, Concat(", \"value\": ", results));
			break;
		}
		case STRING: {
			#ifdef EXPAND_AST
				printf(" %s-value: \"%s\"\n", tabs, node->meta.stringExpr.value);
			#endif
			char temp2[strlen(node->meta.stringExpr.value) + 15];
			sprintf(temp2, ", \"value\": \"%s\"", node->meta.stringExpr.value);
			json = Concat(json, temp2);
			break;
		}
		case BINARY: {
			char* json2 = "";
			json2 = Concat(json2, ", \"operator\": \"");
			json2 = Concat(json2, TokenToString(node->meta.binaryExpr.op));
			json2 = Concat(json2, "\", \"left\": ");
			#ifdef EXPAND_AST
				printf(" %s-operator: %s\n %s-left:\n", tabs, TokenToString(node->meta.binaryExpr.op), tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.binaryExpr.left, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"right\": ");
			#ifdef EXPAND_AST
				printf(" %s-right:\n", tabs);
			#endif
			results = ExpandASTNode(scope, node->meta.binaryExpr.right, tab + 2);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
		case BOOLEAN: {
			#ifdef EXPAND_AST
				printf(" %s-value: %d\n", tabs, node->meta.booleanExpr.value);
			#endif
			char temp2[15];
			sprintf(temp2, ", \"value\": \"%d\"", node->meta.booleanExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case BYTE: {
			#ifdef EXPAND_AST
				printf(" %s-value: %d\n", tabs, node->meta.byteExpr.value);
			#endif
			char temp2[30];
			sprintf(temp2, ", \"value\": \"%d\"", node->meta.byteExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case SHORT: {
			#ifdef EXPAND_AST
				printf(" %s-value: %d\n", tabs, node->meta.shortExpr.value);
			#endif
			char temp2[30];
			sprintf(temp2, ", \"value\": \"%d\"", node->meta.shortExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case INT: {
			#ifdef EXPAND_AST
				printf(" %s-value: %d\n", tabs, node->meta.intExpr.value);
			#endif
			char temp2[60];
			sprintf(temp2, ", \"value\": \"%d\"", node->meta.intExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case FLOAT: {
			#ifdef EXPAND_AST
				printf(" %s-value: %f\n", tabs, node->meta.floatExpr.value);
			#endif
			char temp2[60];
			sprintf(temp2, ", \"value\": \"%f\"", node->meta.floatExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case DOUBLE: {
			#ifdef EXPAND_AST
				printf(" %s-value: %f\n", tabs, node->meta.doubleExpr.value);
			#endif
			char temp2[60];
			sprintf(temp2, ", \"value\": \"%f\"", node->meta.doubleExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case LONG: {
			#ifdef EXPAND_AST
				printf(" %s-value: %ld\n", tabs, node->meta.longExpr.value);
			#endif
			char temp2[60];
			sprintf(temp2, ", \"value\": \"%ld\"", node->meta.longExpr.value);
			json = Concat(json, temp2);
			break;	
		}
		case FOR: {
			char* json2 = "";
			json2 = Concat(json2, ", \"var\": ");
			#ifdef EXPAND_AST
				printf(" %s-var:\n", tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.forExpr.var, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"condition\": ");
			#ifdef EXPAND_AST
				printf(" %s-condition:\n", tabs);
			#endif
			results = ExpandASTNode(scope, node->meta.forExpr.condition, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"incrementor\": ");
			#ifdef EXPAND_AST
				printf(" %s-incrementor:\n", tabs);
			#endif
			results = ExpandASTNode(scope, node->meta.forExpr.inc, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"body\": ");
			#ifdef EXPAND_AST
				printf(" %s-body:\n", tabs);
			#endif
			results = ExpandSubScope(scope, node->meta.forExpr.body, tab);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
		case IF: {
			char* json2 = "";
			json2 = Concat(json2, ", \"condition\": ");
			#ifdef EXPAND_AST
				printf(" %s-condition:\n", tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.ifExpr.condition, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"body\": ");
			#ifdef EXPAND_AST
				printf(" %s-body:\n", tabs);
			#endif
			results = ExpandSubScope(scope, node->meta.ifExpr.body, tab);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
		case WHILE: {
			char* json2 = "";
			json2 = Concat(json2, ", \"condition\": ");
			#ifdef EXPAND_AST
				printf(" %s-condition:\n", tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.whileExpr.condition, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"body\": ");
			#ifdef EXPAND_AST
				printf(" %s-body:\n", tabs);
			#endif
			results = ExpandSubScope(scope, node->meta.whileExpr.body, tab);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
		case SWITCH: {
			char* json2 = "";
			json2 = Concat(json2, ", \"condition\": ");
			#ifdef EXPAND_AST
				printf(" %s-condition:\n", tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.switchExpr.condition, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"body\": ");
			#ifdef EXPAND_AST
				printf(" %s-body:\n", tabs);
			#endif
			results = ExpandSubScope(scope, node->meta.switchExpr.body, tab);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
		case CASE: {
			char* json2 = "";
			json2 = Concat(json2, ", \"condition\": ");
			#ifdef EXPAND_AST
				printf(" %s-condition:\n", tabs);
			#endif
			char* results = ExpandASTNode(scope, node->meta.caseStmt.condition, tab + 2);
			json2 = Concat(json2, results);
			json2 = Concat(json2, ", \"body\": ");
			#ifdef EXPAND_AST
				printf(" %s-body:\n", tabs);
			#endif
			results = ExpandSubScope(scope, node->meta.caseStmt.body, tab);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
		case FUNC: {
			char* json2 = "";
			json2 = Concat(json2, ", \"name\": \"");
			json2 = Concat(json2, node->meta.funcExpr.name);
			json2 = Concat(json2, "\", \"params\": [");
			#ifdef EXPAND_AST
				printf(" %s-name: %s\n %s-Params:\n", tabs, node->meta.funcExpr.name, tabs);
			#endif

			bool first = true;
			char* results = "";
			FOREACH_AST(node->meta.funcExpr.params){
				results = ExpandASTNode(scope, item->node, tab + 2);
				if (first) first = false;
				else json2 = Concat(json2, ",");
				json2 = Concat(json2, results);
			}

			json2 = Concat(json2, "], \"body\": ");

			#ifdef EXPAND_AST
				printf(" %s-body:\n", tabs);
			#endif
			results = ExpandSubScope(scope, node->meta.funcExpr.body, tab);
			json2 = Concat(json2, results);
			json = Concat(json, json2);
			break;
		}
	}
	return Concat(json, "}");
}

ASTNode* FindSymbol(Scope* scope, char* name){
	for (int i = 0; i < scope->nodeLength; i++){
		Token t = scope->nodes[i].type;
		if (t == OBJECT) {
			NOT_IMPLEMENTED("Symbol for OBJECT");
		}
		else if (t == FUNC && strcmp(name, scope->nodes[i].meta.funcExpr.name) == 0){
			return &scope->nodes[i];
		}
		else if (t == VAR && strcmp(name, scope->nodes[i].meta.varExpr.name) == 0){
			return &scope->nodes[i];
		}
	}
	return NULL;
}