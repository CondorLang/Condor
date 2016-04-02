#include "internal.h"

namespace Cobra {
namespace internal{

	void Internal::PrintF(ASTNode* lit){
		ASTLiteral* v = (ASTLiteral*) lit;
		if (v->type == LITERAL) {
			printf("%s", v->value.c_str());
		}
	}

} // namespace internal
}