#ifndef SEMANTICS_H_
#define SEMANTICS_H_

#include "cobra/mem/isolate.h"
#include "cobra/ast/scope.h"
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"
#include "binary.h"

namespace Cobra {
namespace internal{

	class Parser;

	class Semantics
	{
	private: 
		Isolate* isolate;
		std::vector<Scope*> scopes;
		Scope* baseScope;
		Parser* parser;
		ASTObject* kThis;
		bool trace;
		int indent;
		bool inObject;
		bool isThis;

		void Trace(const char* first, const char* second);
		void Trace(const char* first, TOKEN second);
		void Expand(bool idt, ASTNode* node);
		void SetRowCol(ASTNode* node);
		void ScanScope(Scope* scope);
		void ValidateVar(ASTVar* var);
		void ValidateFor(ASTForExpr* expr);
		void ValidateBoolean(ASTBinaryExpr* expr);
		void ValidateIf(ASTIf* expr);
		void ValidateFunc(ASTFunc* func, bool parse = false, bool isConstructor = false);
		TOKEN ValidateExpr(ASTExpr* expr);
		TOKEN ValidateLiteral(ASTLiteral* lit);
		TOKEN ValidateBinary(ASTBinaryExpr* expr);
		TOKEN ValidateIdent(ASTLiteral* expr);
		TOKEN ValidateFuncCall(ASTFuncCall* expr, bool isConstructor = false);
		TOKEN ValidateArray(ASTArray* expr);
		TOKEN ValidateObjectChain(ASTBinaryExpr* expr);
		void ExpectNumber(ASTLiteral* lit);
		void ValidateObject(ASTObject* obj);
		void ValidateObjectInit(ASTVar* var);
		void ValidateExtend(ASTObject* base, ASTObject* extend);
		ASTObject* GetObject(ASTNode* node);
		TOKEN ValidateCast(ASTExpr* expr);
		void OpenScope(Scope* scope){scopes.insert(scopes.begin(), scope);}
		void CloseScope(){scopes.erase(scopes.begin());}
		Scope* GetCurrentScope();
		Scope* GetPreviousScope();
		void SwapScopes();

	public:
		Semantics(Isolate* iso);
		~Semantics();
		static Semantics* New(Isolate* iso, Parser* parser);
		int row;
		int col;
		void Evaluate(Scope* scope);
		std::string* GetSource();
		
	};

} // namespace internal
}

#endif // SEMANTICS_H_
