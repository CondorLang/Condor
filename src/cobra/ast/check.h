#ifndef CHECK_H_
#define CHECK_H_

#include <string>
#include <vector>
#include "../token/token.h"
#include "../ast/scope.h"
#include "../ast/ast.h"
#include "../error/error.h"
#include "../parser/parser.h"

namespace Cobra {
	class Check
	{
	private:
		Scope* scope;
		ASTFile* file;
		bool trace;

		void Trace(std::string msg1, std::string msg2);
		void OpenBaseScope();
		void OpenScope(Scope* scope);
		void CloseScope();
		void CountItemsInScope();
		bool HasMain();
		void ValidateFunc(ASTFunc* func);
		void ValidateFuncArgs(ASTFunc* func);
		void ScanScope();
		void CheckScopeLevelNode(ASTNode* node);
		void ValidateFor(ASTNode* node);
		void ValidateVar(ASTNode* node);
		void ValidateStmt(ASTExpr* expr);
		void ValidateUnaryStmt(ASTUnaryExpr* unary);
		void ValidateLiterary(ASTLiterary* lit);
		void ValidateBinaryStmt(ASTBinaryExpr* binary);
		void ValidateIdent(ASTIdent* ident);
		ASTNode* GetObjectInScope(ASTIdent* ident, Scope* sc);

	public:
		Check();
		~Check();
		int col;
		int row;

		void CheckFile(ASTFile* file);
		void SetOptions(std::string option);
	};
}

#endif // CHECK_H_
