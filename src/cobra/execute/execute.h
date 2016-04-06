#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <string>
#include <vector>

#include "cobra/ast/node.h"
#include "cobra/ast/scope.h"
#include "cobra/mem/isolate.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class Execute
	{
	private:
		std::vector<Scope*> scopes;
		Isolate* isolate;
		bool trace;

		void Trace(std::string first, std::string msg2);
		void OpenScope(Scope* sc);
		void CloseScope(){scopes.erase(scopes.begin());}
		Scope* GetCurrentScope(){return scopes[0];}

		ASTLiteral* EvaluateFuncCall(ASTFuncCall* call);
		ASTLiteral* EvaluateBinary(ASTBinaryExpr* binary); 
		ASTLiteral* EvaluateValue(ASTNode* node);
		ASTLiteral* EvaluateVar(ASTVar* var);

	public:
		static Execute* New(Isolate* isolate, Scope* scope);
		Execute(Scope* scope);
		~Execute(){}
		void Evaluate();
	};

} // namespace internal
} // namespace Cobra

#endif // EXECUTE_H_