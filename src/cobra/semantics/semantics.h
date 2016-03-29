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
		Scope* currentScope;
		Scope* baseScope;
		Parser* parser;
		bool trace;
		int indent;

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
		void ExpectNumber(ASTLiteral* lit);
		void ValidateObject(ASTObject* obj);
		void ValidateObjectInit(ASTVar* var);
		void ValidateExtend(ASTObject* base, ASTObject* extend);

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
