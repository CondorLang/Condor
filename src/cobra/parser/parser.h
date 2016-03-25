#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <stdarg.h>

#include "cobra/mem/isolate.h"
#include "cobra/token/token.h"
#include "cobra/ast/node.h"
#include "cobra/ast/scope.h"
#include "cobra/scanner/scanner.h"
#include "cobra/types/strings/string.h"

namespace Cobra {
namespace internal{

	class Parser
	{
	private:
		Isolate* isolate;
		std::string* source;
		Scanner* scanner;
		Token* tok;
		Scope* rootScope;
		Scope* scope;
		int pos;
		int row;
		int col;
		bool reset;
		bool nonOp;
		bool isInline;
		bool isInternal;
		bool printVariables;
		bool trace;
		std::vector<ASTImport*> imports;
		std::vector<ASTInclude*> includes;

		void SetDefaults(std::string* source);
		void SetRowCol(ASTNode* node);
		void Next();
		void Expect(TOKEN val);
		bool Is(int argc, ...);
		void PrintTok();
		void Trace(const char* name, const char* value);
		void ParseImportOrInclude();
		std::string ParseAlias();
		void ParseShallowStmtList(TOKEN terminator = END);
		bool IsOperator();
		bool IsBoolean();
		bool IsAssignment();
		bool IsVarType();
		ASTFunc* ParseFunc();
		Scope* LazyParseBody();
		ASTNode* ParseIdentStart();
		std::vector<ASTVar*> ParseVarList();
		ASTExpr* ParseExpr();
		ASTExpr* ParseBinaryExpr();
		ASTExpr* ParseVarType();
		ASTExpr* ParseFuncCall(ASTExpr* expr);
		ASTExpr* ParseForExpr();
		ASTExpr* ParseBoolean();
		ASTExpr* ParseWhile();
		ASTExpr* ParseTryCatch();
		std::vector<ASTVar*> ParseFuncArgs();
		ASTExpr* ParseThrow();
		ASTExpr* ParseIf();
		ASTNode* ParseDelete();
		ASTExpr* ParseSwitch();
		ASTExpr* ParseCase();
		ASTNode* ParseObject();
		void OpenScope(Scope* sc);
		void CloseScope();
		ASTNode* ParseInternal();
		ASTNode* ParseReturn();

	public:
		Parser(Isolate* iso);
		~Parser();
		static Parser* New(Isolate* isolate, std::string* source);
		void SetInteral(bool isInternal){isInternal = isInternal;}
		bool IsInternal(){return isInternal;}
		void SetInline(bool isInline){isInline = isInline;}
		void Parse();
		static Scope* Parse(Isolate* iso, Scope* sc);
		Scope* GetBaseScope(){return rootScope;}
		std::string* GetSource(){return source == NULL ? scanner->src : source;}

		int Pos;
		int Row;
		int Col;
		Token* expected;
		
	};
} // namespace internal
}

#endif // PARSER_H_
