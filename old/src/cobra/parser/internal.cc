#include "internal.h"

namespace Cobra {
namespace internal{

	ASTNode* Internal::ParseCall(std::vector<ASTExpr*> params, std::string name, Scope* scope){
		if (name == "getStringLength") return NULL; //return GetStringlength(arg, scope);
		else if (name == "getType") return GetType(params, scope);
		return NULL;
	}

	ASTNode* Internal::GetStringlength(ASTNode* arg, Scope* scope){
		ASTIdent* ident = (ASTIdent*) arg;
		std::vector<ASTNode*> values = scope->Lookup("String");
		printf("%lu\n", values.size());
		ASTLiterary* lit = (ASTLiterary*) values[0];
		return lit;
	}

	ASTNode* Internal::GetType(std::vector<ASTExpr*> params, Scope* scope){
		ASTString* str = ASTString::New(scope->isolate);
		for (int i = 0; i < params.size(); i++){
			ASTNode* node = params[i];
			switch ((int) node->type){
				case IDENT: {
					ASTIdent* ident = (ASTIdent*) node;
					if (ident->value->type == VAR) node = ident->value;
					else break;
				}
				case VAR: {
					ASTVar* var = (ASTVar*) node;
					ASTNode* varClass = var->varClass;
					str->value += varClass->name;
					break;
				}
				case ARRAY: {
					ASTArray* ary = (ASTArray*) node;
					str->value += ary->base->name + "Array";
					break;
				}
				default: {
					str->value += "Unknown";
				}
			}
			if (i + 1 < params.size()) str->value += "|";
		}
		return str;
	}

} // namespace internal
}
