#include "lexer.h"

void InitLexer(Lexer* lexer, char* rawSourceCode){
	// Init lexer
	lexer->rawSourceCode = rawSourceCode;
	lexer->length = strlen(rawSourceCode);
	lexer->currentTokenString = NULL;

	// Init lexer tracker
	lexer->tracker.currentTokenPosition = 0;
	lexer->tracker.tokenStart = 0;
	lexer->tracker.tokenEnd = 0;
	lexer->tracker.row = 1;
	lexer->tracker.col = 1;
}

void DestroyLexer(Lexer* lexer){
	if (lexer->currentTokenString != NULL) Free(lexer->currentTokenString);
}

Token GetNextToken(Lexer* lexer){
	SetNextTokenRange(lexer);
	if (lexer->tracker.tokenEnd - lexer->tracker.tokenStart == 0) return UNDEFINED;
	char* token = GetTokenString(lexer);
	if (lexer->currentTokenString != NULL) {
		Free(lexer->currentTokenString);
	}
	lexer->currentTokenString = token;
	Token tok = StringToToken(token);
	// Reset token locations
	lexer->tracker.tokenStart = 0;
	lexer->tracker.tokenEnd = 0;
	bool containsChars = strlen(token) > 0;
	if (tok == UNDEFINED && containsChars && (token[0] == '.' || isdigit(token[0]))) {
		return NUMBER;
	}
	else if (tok == UNDEFINED && containsChars && (token[0] == '\'' || token[0] == '"')){
		return STRING;
	}
	else if (tok == UNDEFINED && containsChars){
		return IDENTIFIER;
	}
	return tok;
}

void PeekNextToken(Lexer* lexer, PeekedToken* peeked){
	peeked->token = GetNextToken(lexer);
	peeked->raw = lexer->currentTokenString;
	BackOneToken(lexer);
}

Token GetCurrentToken(Lexer* lexer){
	BackOneToken(lexer);
	return GetNextToken(lexer);
}
	
/**
 * This function is key in allocation the correct number of 
 * ASTNodes.
 *
 * Logic:
 * 	If VAR appears, allocate 1 ASTVar slot
 * 	If ASSIGN appears, allocate 1 ASTBinaryExpr slot var a = 10;
 */
int CountTotalASTTokens(Lexer* lexer){
	int total = 0;
	Token tok = GetNextToken(lexer);
	while (tok != UNDEFINED){
		if (tok == VAR ||
			tok == FOR ||
			tok == ASSIGN ||
			tok == NUMBER ||
			tok == STRING || 
			tok == IF ||
			tok == BREAK ||
			tok == SWITCH ||
			tok == CASE ||
			IsBinaryOperator(tok) ||
			IsBooleanOperator(tok)) total++;
		tok = GetNextToken(lexer);
	}
	DestroyLexer(lexer);
	return total;
}

int CountTotalScopes(Lexer* lexer){
	int total = 0;
	int len = strlen(lexer->rawSourceCode);
	for (int i = 0; i < len; i++){
		if (lexer->rawSourceCode[i] == '{') total++;
	}
	DestroyLexer(lexer);
	return total;
}

void BackOneToken(Lexer* lexer){
	lexer->tracker.currentTokenPosition -= strlen(lexer->currentTokenString);
}

void ResetLexer(Lexer* lexer){
	lexer->tracker.tokenStart = 0;
	lexer->tracker.tokenEnd = 0;
	lexer->tracker.currentTokenPosition = 0;
	lexer->currentTokenString = NULL;
}

char* GetCurrentTokenString(Lexer* lexer){
	return lexer->currentTokenString;
}

void SetNextTokenRange(Lexer* lexer){
	CrawlSpaces(lexer); // common
	if (CrawlIdentifierCharacters(lexer) ||
		CrawlNumbers(lexer) ||
		CrawlString(lexer) ||
		CrawlOperators(lexer)){
		// Just need to break
	}
}

bool CrawlOperators(Lexer* lexer){
	char currentChar = GetCurrentCharacter(lexer);
	char nextChar = PeekNextCharacter(lexer);
	SetTokenStart(lexer);
	switch (currentChar){
		case '/': {
			if (nextChar == '/' || nextChar == '*') {} // TODO: Eat Comments
			if (nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '*': {
			if (nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '=': {
			if (nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '+': {
			if (nextChar == '+' || nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '-': {
			if (nextChar == '-' || nextChar == '=' || nextChar == '>') GetNextCharacter(lexer);
			if (isdigit(nextChar) || nextChar == '.') CrawlNumbers(lexer);
			break;
		}
		case '%': {
			if (nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '<': {
			if (nextChar == '-' || nextChar == '=') GetNextCharacter(lexer);
			else if (nextChar == '<') GetNextCharacter(lexer); // TODO: Implement <<=
			break;
		}
		case '>': {
			if (nextChar == '=') GetNextCharacter(lexer);
			else if (nextChar == '>') GetNextCharacter(lexer); // TODO: Implement >>=
			break;
		}
		case '!': {
			if (nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '&': {
			if (nextChar == '&' || nextChar == '^' || nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '|': {
			if (nextChar == '|' || nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '^': {
			if (nextChar == '=') GetNextCharacter(lexer);
			break;
		}
		case '.': {
			if (isdigit(nextChar)) CrawlNumbers(lexer);
			break;
		}
		case '`':
		case '@':
		case '$':
		case ';':
		case ',':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '#':
		case ':': break;
		default: {
			SetTokenEnd(lexer);
			return false;
		}
	}
	GetNextCharacter(lexer);
	SetTokenEnd(lexer);
	return true;
}

bool CrawlString(Lexer* lexer){
	char currentChar = GetCurrentCharacter(lexer);
	if (currentChar != '\'' && currentChar != '"') return false;
	SetTokenStart(lexer);
	char nextChar = PeekNextCharacter(lexer);
	char stringBreak = currentChar == '\'' ? '\'' : '"';
	while (nextChar != stringBreak){
		GetNextCharacter(lexer);
		nextChar = PeekNextCharacter(lexer);
	}
	GetNextCharacter(lexer);
	GetNextCharacter(lexer);
	SetTokenEnd(lexer);
	return true;
}

/**
 * Permitted number patterns:
 * 	([.0-9]+)|([0-9]+\.[0-9]+)
 */
bool CrawlNumbers(Lexer* lexer){
	char currentChar = GetCurrentCharacter(lexer);
	char nextChar = PeekNextCharacter(lexer);
	bool isTrailingPeriod = currentChar == '.';
	if (isdigit(currentChar) || isTrailingPeriod){
		SetTokenStart(lexer);
		bool inside = false;
		while (true){
			if (!isdigit(nextChar) && nextChar != '.') break;
			nextChar = PeekNextCharacter(lexer);
			GetNextCharacter(lexer);
			inside = true;
		}
		if (!inside) GetNextCharacter(lexer);
		SetTokenEnd(lexer);
		return true;
	}
	return false;
}

/**
 * Permitted identifier patterns:
 * 	[a-zA-Z_][a-zA-Z0-9_$]+
 */
bool CrawlIdentifierCharacters(Lexer* lexer){
	char currentChar = GetCurrentCharacter(lexer);
	char nextChar = PeekNextCharacter(lexer);
	if (isalpha(currentChar) || currentChar == '_'){
		SetTokenStart(lexer);
		char* checkCharacters = "$_";
		bool inside = false;
		while (true){
			if (!isalnum(nextChar) && !IsCharacter(nextChar, checkCharacters)) break;
			nextChar = PeekNextCharacter(lexer);
			GetNextCharacter(lexer);
			inside = true;
		}
		if (!inside) GetNextCharacter(lexer);
		SetTokenEnd(lexer);
		return true;
	}
	return false;
}

bool CrawlSpaces(Lexer* lexer){
	char currentChar = GetCurrentCharacter(lexer);
	if (currentChar == ' '){
		char nextChar = PeekNextCharacter(lexer);
		char* checkCharacters = " \n\r";
		while (IsCharacter(nextChar, checkCharacters)){
			if (nextChar == '\n'){
				lexer->tracker.row++;
				lexer->tracker.col = 1;
			}

			nextChar = PeekNextCharacter(lexer);
			GetNextCharacter(lexer);
		}
		GetNextCharacter(lexer);
		return true;
	}
	return false;
}

char GetCurrentCharacter(Lexer* lexer){
	if (lexer->length > lexer->tracker.currentTokenPosition){
		return lexer->rawSourceCode[lexer->tracker.currentTokenPosition];
	}
	return -1; // End of File
}

char GetNextCharacter(Lexer* lexer){
	lexer->tracker.currentTokenPosition++;
	return GetCurrentCharacter(lexer);
}

char PeekNextCharacter(Lexer* lexer){
	lexer->tracker.currentTokenPosition++;
	char c = GetCurrentCharacter(lexer);
	lexer->tracker.currentTokenPosition--;
	return c;
}

bool IsCharacter(char character, char charactersToCheck[]){
	int len = strlen(charactersToCheck);
	for (int i = 0; i < len; i++){
		if (character == charactersToCheck[i]) return true;
	}
	return false;
}

char* GetTokenString(Lexer* lexer){
	LexerTracker tracker = lexer->tracker;
	int len = tracker.tokenEnd - tracker.tokenStart;
	char* placeholder = (char*) Allocate((sizeof(char) * len) + sizeof(char));
	int charSpot = 0;
	for (int i = tracker.tokenStart; i < tracker.tokenEnd; i++){
		placeholder[charSpot++] = lexer->rawSourceCode[i];
	}
	placeholder[len] = '\0';
	return placeholder;
}

void SetTokenStart(Lexer* lexer){
	lexer->tracker.tokenStart = lexer->tracker.currentTokenPosition;
}

void SetTokenEnd(Lexer* lexer){
	lexer->tracker.tokenEnd = lexer->tracker.currentTokenPosition;
}