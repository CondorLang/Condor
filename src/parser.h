#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <stdio.h>
#include <map>
#include "tokens.h"
#include "lexer.h"
#include "error.h"
#include "limits.h"
#include "function.h"
#include "var.h"

namespace Cobra {

	class Parser
	{
	private:
		Lexer* lexer;
		Tokens::Token token;
		std::map<std::string, Function*> functions;

	public:
		Parser(std::string rawCode);
		~Parser();
		int row;
		int col;
		const char* lexeme;
		void nextToken();
		void Parse();
		void ParseFunction();
		void ParseBlock();

		bool exists(const char* name);
	};
}

#endif // PARSER_H_	
