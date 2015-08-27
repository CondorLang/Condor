#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include "cobra/token/token.h"
#include "cobra/scanner/scanner.h"
#include "cobra/ast/scope.h"
#include "cobra/error/error.h"
#include "cobra/ast/ast.h"

namespace Cobra {
namespace internal{

	class Isolate;
	class Scanner;

	enum P_ERROR {
		INVALID_MODE = 0
	};

	/**
	 * Parser mode is either lazy or strict
	 */
	enum P_MODE {
		STRICT,
		LAZY
	};

	/**
	 * @brief The Parser class
	 * @details This is where we create the AST
	 */
	class Parser
	{
	public:
		P_MODE mode;
		int pos;
		int row;
		int col;
		Token* expected;

		Parser(std::string* src, std::string* path);
		~Parser();
		ASTFile* Parse();
		std::vector<ASTImport*> imports;
		std::vector<ASTInclude*> includes;
		std::vector<ASTNode*> exports;
		void SetIsolate(Isolate* iso){isolate = iso;}
		std::string* filePath;

	private:
		Scanner* scanner;
		Token* tok;
		Scope* topScope;
		Scope* currentFunctionScope;
		Isolate* isolate;
		std::string* source;

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
		void AddRowCol(ASTNode* node);
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
} // namespace internal
}

#endif // PARSER_H_
