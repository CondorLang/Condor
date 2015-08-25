#ifndef CHECK_H_
#define CHECK_H_

#include <string>
#include <vector>
#include "cobra/token/token.h"
#include "cobra/ast/scope.h"
#include "cobra/ast/ast.h"
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"

namespace Cobra {
namespace internal{
	class Check
	{
	private:
		Scope* scope;
		ASTFile* file;
		Isolate* isolate;
		bool trace;
		int printIndent;

		void Trace(std::string msg1, std::string msg2);
		std::string GetTokenString(TOKEN tok);
		void OpenBaseScope();
		void OpenScope(Scope* scope);
		void CloseScope();
		void CountItemsInScope();
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
		void ValidateFuncCall(ASTFuncCallExpr* call);
		void ValidateCast(ASTCastExpr* cast);
		bool ValidateObjectChainMember(ASTObjectMemberChainExpr* member);
		ASTNode* GetObjectInScope(ASTIdent* ident, Scope* sc);
		ASTNode* GetObjectInScopeByString(std::string name, Scope* sc);
		void SetScope();

	public:
		Check();
		~Check();
		int col;
		int row;

		void CheckFile(ASTFile* file);
		bool HasMain();
		void SetIsolate(Isolate* iso){isolate = iso;}
	};
} // namespace internal{
}

#endif // CHECK_H_
