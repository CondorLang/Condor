#include "syntax.h"

ASTNode* GetNextNode(Scope* scope){
	int loc = scope->nodeSpot++;
	if (loc >= scope->nodeLength) return NULL;
	return &scope->nodes[loc];
}

ASTNode* ParseFor(Scope* scope, Lexer* lexer){
	DEBUG_PRINT_SYNTAX("For");
	TRACK();
	ASTNode* forExpr = GetNextNode(scope);
	forExpr->isStmt = true;
	forExpr->type = FOR;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, LPAREN, lexer);
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
	EXPECT_TOKEN(tok, RPAREN, lexer);
	forExpr->meta.forExpr.body = ParseBody(scope, lexer);
	return forExpr;
}

ASTNode* ParseIf(Scope* scope, Lexer* lexer){
	DEBUG_PRINT_SYNTAX("If");
	TRACK();
	ASTNode* ifExpr = GetNextNode(scope);
	ifExpr->isStmt = true;
	ifExpr->type = IF;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, LPAREN, lexer);
	ifExpr->meta.ifExpr.condition = ParseExpression(scope, lexer);
	ifExpr->meta.ifExpr.body = ParseBody(scope, lexer);
	return ifExpr;
}

ASTNode* ParseWhile(Scope* scope, Lexer* lexer){
	DEBUG_PRINT_SYNTAX("While");
	TRACK();
	ASTNode* whileStmt = GetNextNode(scope);
	whileStmt->isStmt = true;
	whileStmt->type = WHILE;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, LPAREN, lexer);
	whileStmt->meta.whileExpr.condition = ParseExpression(scope, lexer);
	whileStmt->meta.whileExpr.body = ParseBody(scope, lexer);
	return whileStmt;
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
			case WHILE: {
				node = ParseWhile(scope, lexer);
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
	DEBUG_PRINT_SYNTAX("Body");
	TRACK();
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
		EXPECT_TOKEN(tok, RBRACE, lexer);
	}
	return scope->scopes[loc];
}

ASTNode* ParseVar(Scope* scope, Lexer* lexer, Token dataType){
	DEBUG_PRINT_SYNTAX("Var");
	TRACK();
	ASTNode* var = GetNextNode(scope);
	var->type = VAR;
	var->meta.varExpr.dataType = dataType;

	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, IDENTIFIER, lexer);

	// Set the name of the variable
	char* name = lexer->currentTokenString;
	var->meta.varExpr.name = Allocate((sizeof(char) * strlen(name)) + sizeof(char));
	var->meta.varExpr.value = NULL; // mem issue prevention
	var->isStmt = true; // is statement node
	var->meta.varExpr.inc = UNDEFINED;
	strcpy(var->meta.varExpr.name, name);
	DEBUG_PRINT_SYNTAX2("Var", name);

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
	DEBUG_PRINT_SYNTAX("Expression");
	Token tok = GetNextToken(lexer);
	ASTNode* result = NULL;
	char* value = lexer->currentTokenString;

	if (tok == NUMBER){
		result = GetNextNode(scope);

		// Build the ASTLiteral node
		SetNumberType(result, value);

		tok = GetNextToken(lexer);
	}
	else if (tok == STRING){
		ASTNode* str = GetNextNode(scope);
		str->type = STRING;
		str->meta.stringExpr.value = Allocate((sizeof(char) * strlen(value)) + sizeof(char));

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

		result = str;
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
	else if (tok == TRUE_LITERAL || tok == FALSE_LITERAL){
		result = GetNextNode(scope);
		result->type = BOOLEAN;

		// Build the ASTLiteral node
		result->meta.booleanExpr.value = tok == TRUE_LITERAL;

		tok = GetNextToken(lexer);
	}

	if (IsBinaryOperator(tok) || IsBooleanOperator(tok)){
		int loc = scope->nodeSpot++;
		ASTNode* left = result; // Is this a safe assumption?
		ASTNode* binary = &scope->nodes[loc];
		binary->type = BINARY;
		binary->meta.binaryExpr.op = tok;
		binary->meta.binaryExpr.left = left;
		binary->meta.binaryExpr.right = ParseExpression(scope, lexer);
		result = binary;
	}

	return result;
}