#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <string>

#include "cobra/ast/node.h"
#include "cobra/ast/scope.h"
#include "cobra/mem/isolate.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class Execute
	{
	private:
		Scope* scope;
		Isolate* isolate;

		void EvaluateBinary(ASTBinaryExpr* binary);
		void EvaluateFuncCall(ASTFuncCall* call);

	public:
		static Execute* New(Isolate* isolate, Scope* scope);
		Execute(Scope* scope);
		~Execute(){}
		void Evaluate();
	};

} // namespace internal
} // namespace Cobra

#endif // EXECUTE_H_