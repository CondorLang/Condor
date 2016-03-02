#ifndef LEXER_H_
#define LEXER_H_

#include <string>

#include "cobra/tokens/tokens.h"

class Lexer
{
private:
	int kPos;
	int kRow;
	int kCol;
	int kPrevCol;
	char kCh;
	std::string lit;

	void next();
	void back();
	char peek();
	void eatWhiteSpaces();
	void eatLineComment();
	void eatMultiLineComment();
	Tokens::Value ScanKeywordsOrIdentifiers();
	Tokens::Value ScanNumbers();

public:
	Lexer(){}
	Lexer(std::string* raw);
	~Lexer();
	Tokens::Value Next();
	void Reset();
	std::string* kRaw;

};

#endif // LEXER_H_