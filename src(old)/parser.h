#ifndef PARSER_H_
#define PARSER_H_

#include "tokens.h"
#include "lexer.h"
#include "import.h"
#include "context.h"
#include "include.h"
#include <stdio.h>
#include <string>

namespace Cobra {

	#define SYNTAX_ERROR 1
	#define EXPECTED_END_OF_STRING_ERROR 2
	#define EXPECTED_SEMICOLON_ERROR 3
	#define EOF_ERROR 4
	#define FILE_NOT_EXISTS 100
	#define FILE_UNREADABLE 101
	#define FILE_UNKNOWN 102

	class Parser
	{
	private:
		Lexer* mLexer;
		std::string stack[10];
		const char* mTxt;
		bool same(const char* txt);
		bool isStringStart();
		bool isStringEnd();
		void err(const char* txt);
		void next(bool no_eof);
		void parseReservedWord();
		void parseImportOrInclude(bool import);
		std::string cleanString();
		Context* mContext;
	public:
		Parser(std::string* code, Context* context);
		~Parser();
		void Parse();
		int getRow();
		int getCol();
		const char* expected;
		const char* found;
	};
}

#endif // PARSER_H_