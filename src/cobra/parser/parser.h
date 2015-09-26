#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include "cobra/token/token.h"
#include "cobra/scanner/scanner.h"
#include "cobra/ast/scope.h"
#include "cobra/error/error.h"
#include "cobra/ast/ast.h"
#include "cobra/mem/isolate.h"
#include "cobra/mem/handle.h"
#include "cobra/types/vector/vector.h"
#include "internal.h"

namespace Cobra {
namespace internal{

	class Isolate;
	class Scanner;

	/**
	 * @brief The Parser class
	 * @details This is where we create the AST
	 */
	class Parser
	{
	public:
		P_MODE mode;
		Token* expected;
		int Pos;
		int Row;
		int Col;

		static Parser* New(Isolate* iso, std::string* src, std::string* path);
		ASTFile* Parse();
		Vector<ASTImport*> imports;
		Vector<ASTInclude*> includes;
		Vector<ASTNode*> exports;
		void SetIsolate(Isolate* iso){isolate = iso;}
		void SetInteral(bool i){internal = i;}
		void SetInline(bool isInline){IsInline = isInline;}
		std::string* filePath;

	private:
		Scanner* scanner;
		Token* tok;
		int pos;
		int row;
		int col;
		bool reset;
		bool nonOp;
		bool IsInline;
		Scope* topScope;
		Scope* currentFunctionScope;
		Isolate* isolate;
		std::string* source;
		bool internal;
		void Flush();

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
		ASTFunc* ParseFunc(bool hasFunc);
		void ParseFuncParams(ASTFunc* func);
		ASTBlock* ParseBlock(bool initEat);
		void ParseStmtList();
		ASTNode* ParseStmt();
		ASTNode* ParseVar();
		ASTNode* ParseReturn();
		ASTNode* ParseIdentStart();
		ASTNode* ParseTryCatch();
		ASTExpr* ParseSimpleStmt();
		ASTExpr* ParseExpr();
		ASTExpr* ParseBinaryExpr();
		ASTExpr* ParseUnaryExpr();
		ASTExpr* ParsePrimaryExpr();
		ASTExpr* ParseOperand();
		ASTIdent* ParseIdent(bool eat);
		ASTExpr* ParseFuncCall(ASTExpr* unary);
		ASTExpr* ParseArray(ASTExpr* expr);
		ASTExpr* ParseObjectInit();
		ASTNode* ParseObject();
		ASTIf* ParseIf();
		ASTElse* ParseElse();
		ASTWhile* ParseWhile();
		ASTFor* ParseFor();
		ASTNode* ParseThrow();
	};

	namespace Sizes{
		const int kParser = sizeof(Parser);
	}
} // namespace internal
}

#endif // PARSER_H_
