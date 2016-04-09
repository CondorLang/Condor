#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <string>
#include <vector>
#include <map>

#include "cobra/ast/node.h"
#include "cobra/ast/scope.h"
#include "cobra/mem/isolate.h"
#include "cobra/token/token.h"
#include "cobra/error/error.h"
#include "cobra/semantics/binary.h"

namespace Cobra {
namespace internal{

	class Execute
	{
	private:
		std::vector<Scope*> scopes;
		std::vector<ASTNode*> stack;
		std::vector<ASTNode*> opStack;
		Isolate* isolate;
		bool trace;

		void Trace(std::string first, std::string msg2);
		void OpenScope(Scope* sc);
		void CloseScope(){scopes.erase(scopes.begin());}
		Scope* GetCurrentScope(){return scopes[0];}
		void SetRowCol(ASTNode* node);

		ASTLiteral* EvaluateFuncCall(ASTFuncCall* call);
		ASTLiteral* EvaluateBinary(ASTBinaryExpr* binary); 
		ASTLiteral* EvaluateValue(ASTNode* node);
		ASTLiteral* EvaluateVar(ASTVar* var);
		void EvaluateFor(ASTForExpr* expr);
		void FillPostix(ASTBinaryExpr* binary);
		ASTLiteral* Calculate();
		ASTLiteral* Calc(ASTToken* tok);
		void FormatLit(ASTLiteral* lit);
		void TruncZeros(ASTLiteral* lit);

	public:
		static Execute* New(Isolate* isolate, Scope* scope);
		Execute(Scope* scope);
		~Execute(){}
		void Evaluate();
		std::string GetSource(){return GetCurrentScope()->raw;}

		int row;
		int col;
	};

} // namespace internal
} // namespace Cobra

#endif // EXECUTE_H_