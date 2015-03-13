#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include "tokens.h"
#include <stdio.h>

namespace Cobra {
	class Lexer
	{
	private:
		std::string* mRaw;
		int mPos; // position of string
		int mCol;
		int mRow;
		char mCh;
		std::string mBuffer;
		Tokens::Token* mCurrTok;
		char nextChar();
		char peek();
		void skipSpace();
		void scanLetters();
		void scanNumbers();
		void bufferToToken();

	public:
		Lexer(std::string* code);
		~Lexer();
		void nextToken();
		const char* getTokenText();
	};
}

#endif // LEXER_H_