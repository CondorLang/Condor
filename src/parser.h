#ifndef PARSER_H_
#define PARSER_H_

#include "tokens.h"
#include "lexer.h"
#include <stdio.h>
#include <string>

namespace Cobra {
	class Parser
	{
	private:
		Lexer* mLexer;
	public:
		Parser(std::string* code);
		~Parser();
		
	};
}

#endif // PARSER_H_