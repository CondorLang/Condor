#ifndef PARSER_H_
#define PARSER_H_

#include <string>
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
		P_MODE mode;
		int pos;
		int row;
		int col;
		Token* expected;

		Parser(std::string* source);
		~Parser();
		ASTFile* Parse();
		std::string GetParserOptions();
		std::vector<ASTImport*> imports;
		std::vector<ASTInclude*> includes;
		std::vector<ASTNode*> exports;

	private:
		Scanner* scanner;
		Token* tok;
		Scope* topScope;
		Scope* currentFunctionScope;

		// Parser options
		bool trace;
		bool printVariables;
		bool printCheck;

		void Trace(const char* name, const char* value);
		void PrintTok();
		void OpenScope();
		void CloseScope();
		void Next();
		void Expect(TOKEN val);
		bool IsOperator();
		bool IsAssignment();
		bool IsVarType();
		bool IsBoolean();
		VISIBILITY GetVisibility();
		void ParseOptions();
		void ParseMode();
		void ParseImportOrInclude();
		ASTNode* ParseNodes();
		ASTFunc* ParseFunc();
		void ParseFuncParams(ASTFunc* func);
		ASTBlock* ParseBlock(bool initEat);
		void ParseStmtList();
		ASTNode* ParseStmt();
		ASTNode* ParseVar();
		ASTNode* ParseReturn();
		ASTNode* ParseIdentStart();
		ASTExpr* ParseSimpleStmt();
		ASTExpr* ParseExpr();
		ASTExpr* ParseBinaryExpr();
		ASTExpr* ParseUnaryExpr();
		ASTExpr* ParsePrimaryExpr();
		ASTExpr* ParseOperand();
		ASTIdent* ParseIdent();
		ASTExpr* ParseFuncCall(ASTExpr* unary);
		ASTExpr* ParseArray(ASTExpr* expr);
		ASTNode* ParseObject();
		ASTIf* ParseIf();
		ASTElse* ParseElse();
		ASTWhile* ParseWhile();
		ASTFor* ParseFor();
	};
}

#endif // PARSER_H_
