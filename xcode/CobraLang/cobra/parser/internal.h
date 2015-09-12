#ifndef INTERNAL_H_
#define INTERNAL_H_

#include <string>
#include <vector>
#include "cobra/ast/ast.h"
#include "cobra/ast/scope.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class Internal
	{
	public:
		static ASTExpr* ParseCall(ASTNode* arg, std::string name, Scope* scope);
		static ASTExpr* GetStringlength(ASTNode* arg, Scope* scope);
	};

} // namespace internal
}

#endif // INTERNAL_H_
