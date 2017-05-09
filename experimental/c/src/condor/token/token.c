#include "token.h"

struct TokensArray {
	enum Tokens token;
	char* str;
} TokensArray[TOTAL_TOKENS] = {CREATE_TOKEN_LIST(T_ARRAY)};

Token StringToToken(char* value){
	if (value == NULL || strlen(value) == 0) return UNDEFINED;
	for (int i = 0; i < (int) TOTAL_TOKENS; i++){
		if (strlen(value) == strlen(TokensArray[i].str) && strcmp(value, TokensArray[i].str) == 0){
			return TokensArray[i].token;
		}
	}
	return UNDEFINED;
}

char* TokenToString(Token tok){
	return TokensArray[tok].str;
}

bool IsAssignment(Token tok){
	return tok > BEGIN_ASSIGNMENT && tok < END_ASSIGNMENT;
}

bool IsBinaryOperator(Token tok){
	return tok > BEGIN_BINARY_OPERATOR && tok < END_BINARY_OPERATOR;
}

bool IsBooleanOperator(Token tok){
	return tok > BEGIN_BOOLEAN_COMPARISON && tok < END_BOOLEAN_COMPARISON;
}

bool IsNumber(Token tok){
	return tok > BEGIN_NUMBER && tok < END_NUMBER;
}

bool IsString(Token tok){
	return tok > BEGIN_STRING && tok < END_STRING;
}