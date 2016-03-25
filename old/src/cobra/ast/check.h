#ifndef CHECK_H_
#define CHECK_H_

#include <string>
#include <vector>
#include "cobra/token/token.h"
#include "cobra/ast/scope.h"
#include "cobra/ast/ast.h"
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"
#include "cobra/parser/internal.h"

namespace Cobra {
namespace internal{

	class Check
	{
	private:
		Scope* scope;
		ASTFile* file;
		Isolate* isolate;
		bool trace;
		bool expand;
		int printIndent;
		int expandIndent;
		bool IsInline;
		ASTNode* kThis;
		ASTNode* kPreviousObject;

		void Expand(std::string msg1, std::string msg2);
		void Trace(std::string msg1, std::string msg2);
		std::string GetTokenString(TOKEN tok);
		void OpenBaseScope();
		void OpenScope(Scope* scope);
		void CloseScope();
		void SetRowCol(ASTNode* node);
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
		void ValidateBinaryStmt(ASTBinaryExpr* binary, bool boolean);
		void ValidateIdent(ASTIdent* ident);
		void ValidateFuncCall(ASTFuncCallExpr* call);
		void ValidateCast(ASTCastExpr* cast);
		void ValidateObjectChainMember(ASTObjectMemberChainExpr* member);
		bool ValidateMemberFuncCall(ASTFunc* func, ASTFuncCallExpr* call);
		void ValidateArrayMember(ASTArrayMemberExpr* expr, ASTArray* array);
		void ValidateIsArrayType(ASTIdent* ident);
		void ValidateIf(ASTIf* ifStmt);
		void ValidateObjectInit(ASTObjectInit* init);
		void ValidateObject(ASTObject* obj);
		void ValidateTryCatch(ASTTryCatch* try_catch);
		void ValidateThrow(ASTThrow* expr);
		void ValidateNot(ASTBinaryExpr* binary);
		void ValidateArray(ASTArray* array);
		ASTObjectInit* CallDefaultConstructor(ASTVar* var, ASTExpr* stmt);
		ASTNode* GetObjectInScope(ASTNode* ident, Scope* sc);
		ASTNode* GetObjectInScopeByString(std::string name, Scope* sc);
		void CallInternal(ASTFuncCallExpr* call);
		void SetScope();
		void SetVarClass(ASTVar* var, bool force);
		void ValidateAstNot(ASTNot* astNot);
		void VerifyTypeConversion(ASTVar* var);
		bool ValidateParamsAndArgTypes(ASTNode* callp, ASTNode* funcp);
		ASTObject* GetObjectInNode(ASTNode* node);

	public:
		Check(){}
		static Check* New(Isolate* iso);
		int col;
		int row;
		P_MODE mode;

		void CheckFile(ASTFile* file);
		bool HasMain();
		void SetIsolate(Isolate* iso){isolate = iso;}
		void SetMode(P_MODE m){mode = m;}
		void SetInline(bool isInline){IsInline = isInline;}
	};

	namespace Sizes{
		const int kCheck = sizeof(Check);
	}
} // namespace internal{
}

#endif // CHECK_H_
