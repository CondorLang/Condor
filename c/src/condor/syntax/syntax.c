#include "syntax.h"

void BuildTree(char* rawSourceCode){
	Clock clock;
	StartClock(&clock);

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

	// Pre-allocate the different variable types
	ASTNode nodes[totalNodes];
	InitNodes(nodes, totalNodes);

	// Pre-allocate the different scopes
	int scopes[totalScopes + 1];
	for (int i = 0; i < totalScopes + 1; i++){
		scopes[i] = i + 1;
	}

	// Build the scope
	Scope scope;
	InitScope(&scope);
	scope.nodeLength = totalNodes;
	scope.nodes = nodes;
	scope.scopeLength = totalScopes;
	scope.scopes = scopes;

	// Let's build the tree
	ParseStmtList(&scope, &lexer, scope.scopes[scope.scopeSpot++], false);

	ExpandScope(&scope, 0);


	// Cleanup
	DestroyLexer(&lexer);
	DestroyNodes(nodes, totalNodes);
	// DestroyScope(&scope);

	EndClock(&clock);
	SetClockDifference(&clock);
	printf("Time: %llu nanoseconds\n", GetClockNanosecond(&clock));
}

ASTNode* ParseFor(Scope* scope, Lexer* lexer){
	int loc = scope->nodeSpot++;
	ASTNode* forExpr = &scope->nodes[loc];
	forExpr->isStmt = true;
	forExpr->type = FOR;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, LPAREN);
	tok = GetNextToken(lexer);

	/**
	 * Since this is an internal variable for a for loop,
	 * this will not be a top level statement.
	 */
	ASTNode* var = ParseVar(scope, lexer, tok);

	forExpr->meta.forExpr.var = var;
	forExpr->meta.forExpr.condition = ParseExpression(scope, lexer);
	forExpr->meta.forExpr.inc = ParseExpression(scope, lexer);

	tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, RPAREN);
	forExpr->meta.forExpr.body = ParseBody(scope, lexer);
	return forExpr;
}

ASTNode* ParseIf(Scope* scope, Lexer* lexer){
	int loc = scope->nodeSpot++;
	ASTNode* ifExpr = &scope->nodes[loc];
	ifExpr->isStmt = true;
	ifExpr->type = IF;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, LPAREN);
	ifExpr->meta.ifExpr.condition = ParseExpression(scope, lexer);
	ifExpr->meta.ifExpr.body = ParseBody(scope, lexer);
	return ifExpr;
}

Token ParseStmtList(Scope* scope, Lexer* lexer, int scopeId, bool oneStmt){
	Token tok = GetNextToken(lexer);
	while (tok != UNDEFINED){
		int type = (int) tok;
		ASTNode* node = NULL;
		switch (type){
			case BOOLEAN: case BYTE:
			case SHORT: case INT:
			case FLOAT: case DOUBLE:
			case LONG: case VAR: {
				node = ParseVar(scope, lexer, tok); 
				break;
			}
			case FOR: {
				node = ParseFor(scope, lexer);
				break;
			}
			case IF: {
				node = ParseIf(scope, lexer);
				break;
			}
		}
		if (node != NULL) node->scopeId = scopeId;
		if (oneStmt) break;
		if (tok == RBRACE) return tok;
		tok = GetNextToken(lexer);
	}
	return tok;
}

int ParseBody(Scope* scope, Lexer* lexer){
	int loc = scope->scopeSpot++;
	Token tok = GetNextToken(lexer);

	/**
	 * The the body has a LBRACE, then it 
	 * could have as many statements, however if there
	 * isn't a LBRACE, only 1 statement is allowed
	 */
	bool oneStmt = tok != LBRACE;
	if (oneStmt) tok = GetNextToken(lexer); // eat
	tok = ParseStmtList(scope, lexer, scope->scopes[loc], oneStmt);
	if (!oneStmt){
		EXPECT_TOKEN(tok, RBRACE);
	}
	return scope->scopes[loc];
}

ASTNode* ParseVar(Scope* scope, Lexer* lexer, Token dataType){
	int loc = scope->nodeSpot++;
	scope->nodes[loc].type = VAR;
	scope->nodes[loc].meta.varExpr.dataType = dataType;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, IDENTIFIER);

	// Set the name of the variable
	char* name = lexer->currentTokenString;
	ASTNode* var = &scope->nodes[loc];
	var->meta.varExpr.name = Allocate((sizeof(char) * strlen(name)) + sizeof(char));
	var->meta.varExpr.value = NULL; // mem issue prevention
	var->isStmt = true; // is statement node
	var->meta.varExpr.inc = UNDEFINED;
	strcpy(var->meta.varExpr.name, name);

	tok = GetNextToken(lexer);

	// For empty variable declarations:
	// e.g: var a; or var b;
	if (tok == SEMICOLON) return var;

	// Only assignment operators are valid after a VAR declaration
	if (!IsAssignment(tok)) EXPECT_STRING("Assignment Operator");
	if (tok == ASSIGN){ // var a = 	
		var->meta.varExpr.value = ParseExpression(scope, lexer);
		return var;
	}
	NOT_IMPLEMENTED("Other assignment types");
	// TODO: Implement other assignment operators

	return var;
}

/**
 * This function guides to what is actually being parsed
 */
ASTNode* ParseExpression(Scope* scope, Lexer* lexer){
	Token tok = GetNextToken(lexer);
	ASTNode* result = NULL;
	char* value = lexer->currentTokenString;

	if (tok == NUMBER){
		int loc = scope->nodeSpot++;

		result = &scope->nodes[loc];

		// Build the ASTLiteral node
		SetNumberType(result, value);

		tok = GetNextToken(lexer);
	}
	else if (tok == STRING){
		int loc = scope->nodeSpot++;
		ASTNode* str = &scope->nodes[loc];
		str->type = STRING;
		strcpy(str->meta.stringExpr.value, value);

		// Remove the quotes around the string.
		memmove(
			&str->meta.stringExpr.value[0], 
			&str->meta.stringExpr.value[1], 
			strlen(str->meta.stringExpr.value) - 0);
		memmove(
			&str->meta.stringExpr.value[strlen(str->meta.stringExpr.value) - 1], 
			&str->meta.stringExpr.value[strlen(str->meta.stringExpr.value)], 
			1);

		tok = GetNextToken(lexer);

		if (tok == SEMICOLON){
			result = str;
		}
	}
	else if (tok == IDENTIFIER){
		ASTNode* symbol = FindSymbol(scope, value);
		if (symbol == NULL){
			EXPECT_STRING("Invalid identifier");
		}
		result = symbol;

		tok = GetNextToken(lexer);

		if (tok == SEMICOLON){
			result = symbol;
		}

		if (tok == INC || tok == DEC){
			symbol->meta.varExpr.inc = tok;
		}
	}

	if (IsBinaryOperator(tok) || IsBooleanOperator(tok)){
		int loc = scope->nodeSpot++;
		ASTNode* left = &scope->nodes[loc - 1]; // Is this a safe assumption?
		ASTNode* binary = &scope->nodes[loc];
		binary->type = BINARY;
		binary->meta.binaryExpr.op = tok;
		binary->meta.binaryExpr.left = left;
		binary->meta.binaryExpr.right = ParseExpression(scope, lexer);
		result = binary;
	}

	return result;
}

void Scan(char* rawSourceCode){
	BuildTree(rawSourceCode);
}