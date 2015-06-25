#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <map>
#include <fstream>
#include <stack>
#include "tokens.h"
#include "lexer.h"
#include "error.h"
#include "limits.h"
#include "ast.h"
#include "executer.h"

namespace Cobra {

	class Parser
	{
	private:
		Lexer* lexer;
		Tokens::Token token;
		std::map<std::string, ASTNode*> program;
		std::stack<ASTNode*> stack;
		std::string scope;

	public:
		Parser(std::string rawCode);
		~Parser();
		int row;
		int col;
		const char* lexeme;
		std::string found;
		std::stack<ASTNode*> callStack;
		void nextToken();
		void Parse();
		void ParseFunction();
		void ParseObject();
		ASTBlock* ParseBlock();
		ASTNode* ParseExpression();
		ASTNode* ParseValue(Tokens::Token* tok);
		ASTNode* GetScopeVarByName(std::string name);
		ASTNode* GetVarByName(std::string func, std::string name);
		ASTFunction* GetFunctionByName(std::string name);

		bool Expect(std::string value);
		bool HasFunction(std::string name);
	};
}

#endif // PARSER_H_	
