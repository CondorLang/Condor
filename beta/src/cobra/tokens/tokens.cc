#include <string>

#include "tokens.h"

#define TOK(name, string, precedence) string,
  const char* const Tokens::kNames[NUM_TOKENS] = {
  	TOKEN_LIST(TOK, TOK)
  };
#undef TOK

Tokens::Value Tokens::GetToken(const char* name){
	for (int i = 0; i < NUM_TOKENS; i++){
		if (strcmp(name, kNames[i]) == 0){
			return (Tokens::Value) i;
		}
	}
	return LITERAL;
}