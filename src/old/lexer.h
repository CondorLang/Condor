#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <stdio.h>
#include "tokens.h"

namespace Cobra {
	class Lexer
	{
	private:
		int pos;
		int rawCodeLength;
		bool hasError;
		std::string rawCode;

		char peek();
		char nextChar();
		void eatSpaces();
		void eatComments();

	public:
		Lexer(std::string code);
		~Lexer();
		void nextToken(Tokens::Token* tok);
		int col;
		int row;
	};
}

#endif // LEXER_H_
