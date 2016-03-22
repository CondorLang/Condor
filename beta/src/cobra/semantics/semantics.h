#ifndef SEMANTICS_H_
#define SEMANTICS_H_

#include "cobra/mem/isolate.h"
#include "cobra/ast/scope.h"
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"

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
		void SetRowCol(ASTNode* node);
		void ScanScope(Scope* scope);
		void ValidateVar(ASTVar* var);
		void ValidateFor(ASTForExpr* expr);
		void ValidateBoolean(ASTBinaryExpr* expr);
		TOKEN ValidateExpr(ASTExpr* expr);
		TOKEN ValidateLiteral(ASTLiteral* lit);

	public:
		Semantics(Isolate* iso);
		~Semantics();
		static Semantics* New(Isolate* iso, Parser* parser);
		int row;
		int col;
		void Evaluate(Scope* scope);
		
	};

} // namespace internal
}

#endif // SEMANTICS_H_
