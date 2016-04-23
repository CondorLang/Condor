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

	ASTNode* Internal::GetStringLength(Isolate* iso, ASTNode* lit){
		if (lit == NULL && lit->type != LITERAL) return NULL;
		ASTLiteral* v = (ASTLiteral*) lit;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = INT;
		int len = (int) v->value.length();
		l->value = std::to_string(len);
		l->calc = (double) len;
		return l;
	}

	ASTNode* Internal::GetLiteralType(Isolate* iso, ASTNode* lit){
		if (lit == NULL && lit->type != LITERAL) return NULL;
		ASTLiteral* v = (ASTLiteral*) lit;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = STRING;
		l->value = Token::ToString(v->litType);
		return l;
	}

	TOKEN Internal::Bind(ASTFuncCall* call){
		if (call->name == "printf") {
			call->callback = Internal::PrintF;
			return UNDEFINED;
		}
		else if (call->name == "readln") {
			call->callback = Internal::ReadLine;
			return STRING;
		}
		else if (call->name == "getStringLength"){
			call->callback = Internal::GetStringLength;
			return INT;
		}
		else if (call->name == "getLitType"){
			call->callback = Internal::GetLiteralType;
			return STRING;
		}
		else throw Error::UNDEFINED_FUNC;
	}

} // namespace internal
}