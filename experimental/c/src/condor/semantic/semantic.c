#include "semantic.h"

#include <stdio.h>

void Scan(char* rawSourceCode){
	BuildTree(rawSourceCode);
}

/**
 * Build the abstract syntax tree for saving
 */
void BuildTree(char* rawSourceCode){
	Clock clock;
	StartClock(&clock);

	DEBUG_PRINT("\n\n------Starting Program------\n");

	// Build Lexer
	Lexer lexer;
	InitLexer(&lexer, rawSourceCode);

	// Pre-allocate memory
	// The heap is 3,000% slower than just using stack
	// memory. So we are pre-allocating the memory
	// using the stack
	int totalNodes = CountTotalASTTokens(&lexer);
	ResetLexer(&lexer);
	int totalScopes = CountTotalScopes(&lexer);
	ResetLexer(&lexer);
	int totalFuncs = CountTotalFuncs(&lexer);
	ResetLexer(&lexer);
	int totalParamItems = CountTotalParamItems(&lexer);
	ResetLexer(&lexer);

	// Pre-allocate the different param lists
	ASTList params[totalFuncs];
	ASTListItem paramItems[totalParamItems];
	InitParams(params, totalFuncs);
	InitParamItems(paramItems, totalParamItems);

	// Pre-allocate the different variable types
	ASTNode nodes[totalNodes];
	InitNodes(nodes, totalNodes);

	// Pre-allocate the different scopes
	int scopes[totalScopes + 1];
	for (int i = 0; i < totalScopes + 1; i++) scopes[i] = i + 1;

	// Build the scope
	Scope scope;
	InitScope(&scope);
	scope.nodeLength = totalNodes;
	scope.nodes = nodes;
	scope.scopeLength = totalScopes;
	scope.scopes = scopes;

	scope.params = params;
	scope.paramItems = paramItems;
	scope.paramsLength = totalFuncs;
	scope.paramItemsLength = totalParamItems;

	// Let's build the tree
	ParseStmtList(&scope, &lexer, scope.scopes[scope.scopeSpot++], false);
	scope.nodeSpot = 0;
	EnsureSemantics(&scope, 1);

	char* json = ExpandScope(&scope, 0);
	WriteToFile("compiled.cd", json);


	// Cleanup
	DestroyLexer(&lexer);
	DestroyNodes(nodes, totalNodes);
	DestroyScope(&scope);

	EndClock(&clock);
	SetClockDifference(&clock);
	DEBUG_PRINT("\n\n------Program Completed------\n");
	DEBUG_PRINT("\n\n------Program Stats------\n");
	printf("Time: %llu nanoseconds\n", GetClockNanosecond(&clock));
}

/**
 * Ensure that the semantics of the inserted code is correct
 */
void EnsureSemantics(Scope* scope, int scopeId){
	ASTNode* node = GetNextNode(scope);

	if (node == NULL) return; // Done

	while (node != NULL){

		// wrong scope
		if (node->scopeId != scopeId) {
			node = GetNextNode(scope);
			continue;
		}

		int type = (int) node->type;

		switch (type){
			case VAR: {
				PredictVarType(node);
				break;
			}
			case FOR: {
				DEBUG_PRINT("Ensuring semantics for FOR loop");
				PredictVarType(GET_FOR_VAR(node));
				if (GetBinaryType(GET_FOR_CONDITION(node)) != BOOLEAN){
					SEMANTIC_ERROR("Invalid For loop condition");
				}
				EnsureSemanticsForBody(scope, GET_FOR_BODY(node));
				break;
			}
			case WHILE: {
				DEBUG_PRINT("Ensuring semantics for WHILE loop");
				if (GetBinaryType(GET_WHILE_CONDITION(node)) != BOOLEAN){
					SEMANTIC_ERROR("Invalid While loop condition");
				}
				EnsureSemanticsForBody(scope, GET_WHILE_BODY(node));
				break;
			}
			case IF: {
				DEBUG_PRINT("Ensuring semantics for IF statement");
				if (GetBinaryType(GET_IF_CONDITION(node)) != BOOLEAN){
					SEMANTIC_ERROR("Invalid If condition");
				}
				EnsureSemanticsForBody(scope, GET_IF_BODY(node));
				break;
			}
		}

		node = GetNextNode(scope);
	}
}

/**
 * Crawl for the body and reset the location when completed
 */
void EnsureSemanticsForBody(Scope* scope, int scopeId){
	int loc = scope->nodeSpot;
	EnsureSemantics(scope, scopeId);
	scope->nodeSpot = loc;
}

/**
 * Predict the variable type, if possible
 */
void PredictVarType(ASTNode* node){
	DEBUG_PRINT3("Predicting Variable Type", GET_VAR(node).name, TokenToString(GET_VAR(node).dataType));

	if (node->type != VAR) return; // already assigned

	ASTNode* value = GET_VAR_VALUE(node);
	if (value == NULL) {
		DEBUG_PRINT("Unable to predict variable type");
	}
	else if (IsNumber(value->type) || IsString(value->type)){
		GET_VAR(node).dataType = value->type;
		DEBUG_PRINT2("Variable Type Set", TokenToString(GET_VAR(node).dataType));
	}
	else if (value->type == BINARY){
		GET_VAR(node).dataType = GetBinaryType(value);
		DEBUG_PRINT2("Variable Type Set", TokenToString(GET_VAR(node).dataType));
	}
}
