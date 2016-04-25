#include "internal.h"

#include "cobra/types/fs/fs.h" // to prevent a million errors

namespace Cobra {
namespace internal{

	ASTNode* Internal::PrintF(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = (ASTLiteral*) lits[0];
		if (v->type == LITERAL) {
			printf("%s", v->value.c_str());
		}
		return NULL;
	}

	ASTNode* Internal::ReadLine(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() > 0) Internal::PrintF(iso, lits);
		ASTLiteral* l = ASTLiteral::New(iso);
		std::string result;
		std::getline (std::cin, result);
		l->value = result;
		return l;
	}

	ASTNode* Internal::GetStringLength(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = (ASTLiteral*) lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = INT;
		int len = (int) v->value.length();
		l->value = std::to_string(len);
		l->calc = (double) len;
		return l;
	}

	ASTNode* Internal::GetLiteralType(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = (ASTLiteral*) lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = STRING;
		l->value = Token::ToString(v->litType);
		return l;
	}

	ASTNode* Internal::ReadFile(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = (ASTLiteral*) lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = STRING;
		l->value = FS::ReadFile(v->value);
		return l;
	}

	ASTNode* Internal::FileExists(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = (ASTLiteral*) lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = BOOLEAN;
		bool does = FS::FileExists(v->value);
		if (does) l->value = "true";
		else l->value = "false";
		return l;
	}

	ASTNode* Internal::WriteFile(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 2) return NULL;
		ASTLiteral* path = (ASTLiteral*) lits[0];
		ASTLiteral* contents = (ASTLiteral*) lits[1];
		FS::WriteFile(path->value, contents->value);
		return NULL;
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
		else if (call->name == "readFile"){
			call->callback = Internal::ReadFile;
			return STRING;
		}
		else if (call->name == "fileExists"){
			call->callback = Internal::FileExists;
			return BOOLEAN;
		}
		else if (call->name == "writeFile"){
			call->callback = Internal::WriteFile;
			return UNDEFINED;
		}
		else throw Error::UNDEFINED_FUNC;
	}

} // namespace internal
}