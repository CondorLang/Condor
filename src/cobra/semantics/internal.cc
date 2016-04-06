#include "internal.h"

namespace Cobra {
namespace internal{

	ASTNode* Internal::PrintF(Isolate* iso, ASTNode* lit){
		if (lit == NULL) return NULL;
		ASTLiteral* v = (ASTLiteral*) lit;
		if (v->type == LITERAL) {
			printf("%s", v->value.c_str());
		}
		return NULL;
	}

	ASTNode* Internal::ReadLine(Isolate* iso, ASTNode* lit){
		ASTLiteral* l = ASTLiteral::New(iso);
		std::string result;
		std::getline (std::cin, result);
		l->value = result;
		return l;
	}

} // namespace internal
}