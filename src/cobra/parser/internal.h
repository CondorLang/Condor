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
		static ASTNode* ParseCall(std::vector<ASTExpr*> params, std::string name, Scope* scope);
		static ASTNode* GetStringlength(ASTNode* arg, Scope* scope);
		static ASTNode* GetType(std::vector<ASTExpr*> params, Scope* scope);
	};

} // namespace internal
}

#endif // INTERNAL_H_
