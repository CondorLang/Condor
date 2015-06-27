#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <fstream>
#include <streambuf>
#include "../token/token.h"
#include "../scanner/scanner.h"
#include "../ast/scope.h"
#include "../ast/ast.h"
#include "../error/error.h"

namespace Cobra {
	enum P_ERROR {
		INVALID_MODE = 0
	};

	enum P_MODE {
		STRICT,
		LAZY
	};

	class Parser
	{
	public:
		Parser(std::string source);
		~Parser();
		Scanner* scanner;
		int pos;
		int row;
		int col;
		Token* tok;
		Scope* topScope;
		Scope* currentFunctionScope;
		P_MODE mode;
		Token* expected;
		std::vector<std::string> imports;
		bool trace;

		void Trace(const char* name, const char* value);
		void OpenScope();
		void CloseScope();
		void Next();
		void Expect(TOKEN val);
		bool IsOperator();
		void Parse();
		void ParseMode();
		void ParseImportOrInclude();
		ASTNode* ParseNodes();
		ASTFunc* ParseFunc();
		void ParseFuncParams(ASTFunc* func);
		ASTBlock* ParseBlock();
		void ParseStmtList();
		ASTNode* ParseStmt();
		ASTNode* ParseVar();
		ASTNode* ParseReturn();
		ASTExpr* ParseSimpleStmt();
		ASTExpr* ParseExpr();
		ASTExpr* ParseBinaryExpr();
		ASTExpr* ParseUnaryExpr();
		ASTExpr* ParsePrimaryExpr();
		ASTExpr* ParseOperand();
		ASTExpr* ParseIdent();
	};
}

#endif // PARSER_H_
