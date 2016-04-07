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
		if (lit->type == LITERAL) Internal::PrintF(iso, lit);
		ASTLiteral* l = ASTLiteral::New(iso);
		std::string result;
		std::getline (std::cin, result);
		l->value = result;
		return l;
	}

	// TODO: replace with a macro
	TOKEN Internal::Bind(ASTFuncCall* call){
		if (call->name == "printf") {
			call->callback = Internal::PrintF;
			return UNDEFINED;
		}
		else if (call->name == "readln") {
			call->callback = Internal::ReadLine;
			return STRING;
		}
		else throw Error::UNDEFINED_FUNC;
		return UNDEFINED;
	}

} // namespace internal
}