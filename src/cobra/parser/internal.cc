#include "internal.h"

namespace Cobra {
namespace internal{

	ASTExpr* Internal::ParseCall(ASTNode* arg, std::string name, Scope* scope){
		if (name == "getStringLength"){
			return GetStringlength(arg, scope);
		}
		return NULL;
	}

	ASTExpr* Internal::GetStringlength(ASTNode* arg, Scope* scope){
		ASTIdent* ident = (ASTIdent*) arg;
		std::vector<ASTNode*> values = scope->Lookup("String");
		printf("%lu\n", values.size());
		ASTLiterary* lit = (ASTLiterary*) values[0];
		return lit;
	}

} // namespace internal
}
