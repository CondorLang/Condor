#ifndef SYNTAX_H_
#define SYNTAX_H_

#include <string>

#include "cobra/lexer/lexer.h"
#include "cobra/tokens/tokens.h"

class Syntax
{
private:
	std::string* kRaw;
	Lexer kLexer;
	Tokens::Value kTok;

	void next();
	void print(Tokens::Value tok);

public:
	Syntax(std::string* raw);
	~Syntax();
	void Check();
};

#endif // SYNTAX_H_