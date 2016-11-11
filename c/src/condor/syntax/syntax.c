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
	ASTNodeMemoryAudit audit;
	InitAudit(&audit);
	CountTotalASTTokens(&lexer, &audit);
	ResetLexer(&lexer);

	// Pre-allocate the different variable types
	ASTVar vars[audit.var];
	ASTBinaryExpr binarys[audit.binary];
	ASTLiteral lits[audit.lit];
	InitVars(vars, audit.var);
	InitBinaryExprs(binarys, audit.binary);
	InitLiterals(lits, audit.lit);

	// Build the scope
	Scope scope;
	InitScope(&scope);
	scope.audit = &audit;
	scope.vars = vars;
	scope.binarys = binarys;
	scope.lits = lits;

	// // Let's build the tree
	Token tok = GetNextToken(&lexer);
	while (tok != UNDEFINED){
		int type = (int) tok;
		switch (type){
			case VAR: ParseVar(&scope, &lexer, tok); break;
		}
		tok = GetNextToken(&lexer);
	}

	for (int i = 0; i < audit.var; i++){
		ASTVar v = scope.vars[i];
		printf("dd: %d - %s\n", v.meta.id, v.name);
	}

	// Cleanup
	DestroyLexer(&lexer);
	DestroyScope(&scope);

	EndClock(&clock);
	SetClockDifference(&clock);
	printf("Time: %llu nanoseconds\n", GetClockNanosecond(&clock));
}

void ParseVar(Scope* scope, Lexer* lexer, Token dataType){
	int loc = scope->varSpot;
	scope->varSpot++;

	scope->vars[loc].dataType = dataType;
	Token tok = GetNextToken(lexer);
	EXPECT_TOKEN(tok, IDENTIFIER);

	// Set the name of the variable
	char* name = lexer->currentTokenString;
	scope->vars[loc].name = Allocate((sizeof(char) * strlen(name)) + sizeof(char));
	strcpy(scope->vars[loc].name, name);

	tok = GetNextToken(lexer);

	// For empty variable declarations:
	// e.g: var a; or var b;
	if (tok == SEMICOLON) return;
	// Only assignment operators are valid after a VAR declaration
	if (!IsAssignment(tok)) EXPECT_STRING("Assignment Operator");
	if (tok == ASSIGN){ // var a = 	
		scope->varSpot--;
		scope->vars[loc].value = ParseExpression(scope, lexer);
		scope->varSpot++;
		return;
	}
	NOT_IMPLEMENTED("Other assignment types");
	// TODO: Implement other assignment operators

}

/**
 * This function guides to what is actually being parsed
 */
void* ParseExpression(Scope* scope, Lexer* lexer){
	Token tok = GetNextToken(lexer);
	if (tok == NUMBER){
		char* value = lexer->currentTokenString;
		int loc = scope->audit->lit;
		scope->audit->lit++;

		// Build the ASTLiteral node
		Token dataType = GetNumberType(value);
		scope->lits[loc].dataType = dataType;
		scope->lits[loc].value = Allocate((sizeof(char) * strlen(value)) + sizeof(char));
		strcpy(scope->lits[loc].value, value);

		tok = GetNextToken(lexer);

		if (tok == SEMICOLON) {
			return &scope->lits[loc];
		}
	}
	return NULL;
}

void Scan(char* rawSourceCode){
	BuildTree(rawSourceCode);
}