#include "check.h"
#include "cobra/mem/isolate.h"
#include "cobra/flags.h"

namespace Cobra{
namespace internal{

	Check::Check(){
		scope = NULL;
		trace = TRACE_CHECK; // debug
		col = 0;
		row = 0;
		printIndent = 0;
		file = NULL;
	}

	Check::~Check(){}

	void Check::CheckFile(ASTFile* f){
		file = f;
		OpenBaseScope();
		CountItemsInScope();
		ScanScope();
		SetScope();
	}

	std::string Check::GetTokenString(TOKEN tok){
		Token* token = new Token(tok);
		std::string result = token->String();
		delete token;
		return result;
	}

	void Check::SetRowCol(ASTNode* node){
		if (node != NULL){
			row = node->row;
			col = node->col;
		}
	}

	void Check::Trace(std::string msg1, std::string msg2){
		std::string ident = "";
		for (int i = 0; i < printIndent; i++){
			ident += "  ";
			if (i == printIndent - 1){
				ident += "- ";
			}
		}
		printf("%s%s - %s\n", ident.c_str(), msg1.c_str(), msg2.c_str());
	}

	ASTNode* Check::GetObjectInScopeByString(std::string name, Scope* sc){
		if (sc == NULL){
			return NULL;
		}
		std::vector<ASTNode*> nodes = sc->Lookup(name);
		ASTNode* node = NULL;
		if (nodes.empty()){
			node = GetObjectInScopeByString(name, sc->outer);
		}
		else{
			node = nodes[0];
		}
		return node;
	}

	ASTNode* Check::GetObjectInScope(ASTNode* ident, Scope* sc){
		SetRowCol(ident);
		if (sc == NULL || ident == NULL){
			return NULL;
		}
		std::vector<ASTNode*> nodes = sc->Lookup(ident->name);
		ASTNode* node = NULL;
		if (nodes.empty() && sc->outer != NULL){
			node = GetObjectInScope(ident, sc->outer);
		}
		else{
			if (ident->type == FUNC || ident->type == FUNC_CALL){
				ASTFuncCallExpr* funcCall = (ASTFuncCallExpr*) ident;
				for (int i = 0; i < nodes.size(); i++){
					if (nodes[i]->type != FUNC) continue;
					ASTFunc* func = (ASTFunc*) nodes[i];
					if (func->args.size() != funcCall->params.size()) continue;
					bool found = true;
					// for (int j = 0; j < func->args.size(); j++){
					// 	if (func->args[i]->type != funcCall->params[i]->type){
					// 		found = false;
					// 		break;
					// 	}
					// }
					if (found){
						node = func;
						break;
					}
				}
			}
			else if (ident->type == IDENT){
				if (nodes.size() > 1){
					for (int i = 0; i < nodes.size(); i++){
						if (nodes[i]->type == VAR || nodes[i]->type == ASTPARAM_VAR){
							return nodes[i];
						}
					}
				}
				else if (nodes.empty()) throw Error::UNDEFINED_VARIABLE;
				node = nodes[0];
				if (node->name == ident->name) return node;
			}
			else{
				throw Error::MULTIPLE_DECL_OF_OBJECT;
			}
		}
		return node;
	}

	void Check::ValidateFuncCall(ASTFuncCallExpr* call){
		SetRowCol(call);
		if (trace) Trace("Calling func", call->name);
		ASTFunc* func = (ASTFunc*) GetObjectInScope(call, scope);
		if (func == NULL) throw Error::UNDEFINED_FUNC;
		func->used = true;
		SetRowCol(func);
		call->func = func;
		printIndent++;
		for (int i = 0; i < call->params.size(); i++){
			ValidateStmt(call->params[i]);
		}
		printIndent--;
	}

	void Check::ValidateIdent(ASTIdent* ident){
		SetRowCol(ident);
		ASTNode* ptr = GetObjectInScope(ident, scope);
		if (ptr->type == VAR) {
			ASTVar* var = (ASTVar*) ptr;
			ident->assignType = var->varType;
			ptr->used = true;
		}
		if (ptr == NULL) throw Error::UNDEFINED_VARIABLE;
		else{
			ident->value = ptr;
			if (ident->inc){
				if (ident->post){
					if (trace) Trace("inc", ident->name + "++");
				}
				else if (ident->pre){
					if (trace) Trace("inc", "++" + ident->name);
				}
			}
			else if (ident->dec){
				if (ident->post){
					if (trace) Trace("inc", ident->name + "--");
				}
				else if (ident->pre){
					if (trace) Trace("inc", "--" + ident->name);
				}
			}
			else{
				if (trace) Trace(ptr->name, "is a " + GetTokenString(ptr->type));
			}
		}
	}

	void Check::ValidateBinaryStmt(ASTBinaryExpr* binary){
		SetRowCol(binary);
		if (binary == NULL){
			if (trace) Trace("Binary", "is NULL");
		}
		else if (binary->op == NULL){

		}
		else{;
			if (binary->Left != NULL) ValidateStmt(binary->Left);
			if (trace) Trace("Operator found", binary->op->String());
			if (binary->Right != NULL) ValidateStmt(binary->Right);
			int type = (int) binary->Left->assignType;
			std::string op = binary->op->String();
			switch (type){
				case INT: case BOOLEAN: {
					if (op == "+"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) binary->assignType = INT;
						else if (binary->Right->assignType == STRING) binary->assignType = STRING;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
						else {
							throw Error::INVALID_ADD_EXPR;
						}
						break;
					}
					else if (op == "-"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
						else {
							throw Error::INVALID_SUB_EXPR;
						}
						break;
					}
					else if (op == "/"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
						else {
							throw Error::INVALID_DIV_EXPR;
						}
						break;
					}
					else if (op == "*"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
						else {
							throw Error::INVALID_MULT_EXPR;
						}
						break;
					}
					else if (op == "%"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
						else {
							throw Error::INVALID_MOD_EXPR;
						}
						break;
					}
				}
				case DOUBLE: {
					if (op == "+"){
						if (binary->Right->assignType == INT) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == CHAR) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == STRING) binary->assignType = STRING;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
						else {
							throw Error::INVALID_ADD_EXPR;
						}
						break;
					}
					else if (op == "-"){
						if (binary->Right->assignType == INT) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
						else {
							throw Error::INVALID_SUB_EXPR;
						}
						break;
					}
					else if (op == "/"){
						if (binary->Right->assignType == INT) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
						else {
							throw Error::INVALID_DIV_EXPR;
						}
						break;
					}
					else if (op == "*"){
						if (binary->Right->assignType == INT) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
						else {
							throw Error::INVALID_MULT_EXPR;
						}
						break;
					}
					else if (op == "%"){
						if (binary->Right->assignType == INT) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
						else {
							throw Error::INVALID_MOD_EXPR;
						}
						break;
					}
				}
				case FLOAT: {
					if (op == "+"){
						if (binary->Right->assignType == INT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = FLOAT;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = FLOAT;
						else if (binary->Right->assignType == CHAR) binary->assignType = FLOAT;
						else if (binary->Right->assignType == STRING) binary->assignType = STRING;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
						else {
							throw Error::INVALID_ADD_EXPR;
						}
						break;
					}
					else if (op == "-"){
						if (binary->Right->assignType == INT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = FLOAT;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = FLOAT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
						else {
							throw Error::INVALID_SUB_EXPR;
						}
						break;
					}
					else if (op == "/"){
						if (binary->Right->assignType == INT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = FLOAT;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = FLOAT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
						else {
							throw Error::INVALID_DIV_EXPR;
						}
						break;
					}
					else if (op == "*"){
						if (binary->Right->assignType == INT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = FLOAT;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = FLOAT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
						else {
							throw Error::INVALID_MULT_EXPR;
						}
						break;
					}
					else if (op == "%"){
						if (binary->Right->assignType == INT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = FLOAT;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = FLOAT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
						else {
							throw Error::INVALID_MOD_EXPR;
						}
						break;
					}
				}
				case CHAR: {
					if (op == "+"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) binary->assignType = STRING;
						else if (binary->Right->assignType == STRING) binary->assignType = STRING;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
						else {
							throw Error::INVALID_ADD_EXPR;
						}
						break;
					}
					else if (op == "-"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
						else {
							throw Error::INVALID_SUB_EXPR;
						}
						break;
					}
					else if (op == "/"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
						else {
							throw Error::INVALID_DIV_EXPR;
						}
						break;
					}
					else if (op == "*"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
						else {
							throw Error::INVALID_MULT_EXPR;
						}
						break;
					}
					else if (op == "%"){
						if (binary->Right->assignType == INT) binary->assignType = INT;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = DOUBLE;
						else if (binary->Right->assignType == FLOAT) binary->assignType = FLOAT;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = INT;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
						else {
							throw Error::INVALID_MOD_EXPR;
						}
						break;
					}
				}
				case STRING: {
					if (op == "+"){
						if (binary->Right->assignType == INT) binary->assignType = STRING;
						else if (binary->Right->assignType == DOUBLE) binary->assignType = STRING;
						else if (binary->Right->assignType == FLOAT) binary->assignType = STRING;
						else if (binary->Right->assignType == BOOLEAN) binary->assignType = STRING;
						else if (binary->Right->assignType == CHAR) binary->assignType = STRING;
						else if (binary->Right->assignType == STRING) binary->assignType = STRING;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
						else {
							throw Error::INVALID_ADD_EXPR;
						}
						break;
					}
					else if (op == "-"){
						if (binary->Right->assignType == INT) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == DOUBLE) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == FLOAT) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == BOOLEAN) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
						else if (binary->Right->assignType == CHAR) binary->assignType = STRING;
						else if (binary->Right->assignType == STRING) binary->assignType = STRING;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
						else {
							throw Error::INVALID_SUB_EXPR;
						}
						break;
					}
					else if (op == "/"){
						if (binary->Right->assignType == INT) throw Error::CANNOT_DIV_INT_FROM_TYPE;
						else if (binary->Right->assignType == DOUBLE) throw Error::CANNOT_DIV_DOUBLE_FROM_TYPE;
						else if (binary->Right->assignType == FLOAT) throw Error::CANNOT_DIV_FLOAT_FROM_TYPE;
						else if (binary->Right->assignType == BOOLEAN) throw Error::CANNOT_DIV_INT_FROM_TYPE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
						else {
							throw Error::INVALID_DIV_EXPR;
						}
						break;
					}
					else if (op == "*"){
						if (binary->Right->assignType == INT) throw Error::CANNOT_MULT_INT_FROM_TYPE;
						else if (binary->Right->assignType == DOUBLE) throw Error::CANNOT_MULT_DOUBLE_FROM_TYPE;
						else if (binary->Right->assignType == FLOAT) throw Error::CANNOT_MULT_FLOAT_FROM_TYPE;
						else if (binary->Right->assignType == BOOLEAN) throw Error::CANNOT_MULT_INT_FROM_TYPE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
						else {
							throw Error::INVALID_MULT_EXPR;
						}
						break;
					}
					else if (op == "%"){
						if (binary->Right->assignType == INT) throw Error::CANNOT_MOD_INT_FROM_TYPE;
						else if (binary->Right->assignType == DOUBLE) throw Error::CANNOT_MOD_DOUBLE_FROM_TYPE;
						else if (binary->Right->assignType == FLOAT) throw Error::CANNOT_MOD_FLOAT_FROM_TYPE;
						else if (binary->Right->assignType == BOOLEAN) throw Error::CANNOT_MOD_INT_FROM_TYPE;
						else if (binary->Right->assignType == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
						else if (binary->Right->assignType == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
						else if (binary->Right->assignType == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
						else {
							throw Error::INVALID_MOD_EXPR;
						}
						break;
					}
				}
			}
		}
	}

	void Check::ValidateLiterary(ASTLiterary* lit){
		SetRowCol(lit);
		lit->assignType = lit->kind;
		if (trace) Trace("Lit value", lit->value);
	}

	void Check::ValidateUnaryStmt(ASTUnaryExpr* unary){
		SetRowCol(unary);
		if (unary == NULL){
			if (trace) Trace("Unary", "is NULL");
		}
		else{
			if (trace) Trace("Operator found", unary->op->String());
			ValidateStmt(unary->value);
			unary->assignType = unary->value->assignType;
		}
	}

	void Check::ValidateStmt(ASTExpr* expr){
		SetRowCol(expr);
		if (expr == NULL) return;
		int type = (int) expr->type;
		switch (type){
			case UNARY: {
				ValidateUnaryStmt((ASTUnaryExpr*) expr);
				break;
			}
			case BINARY: {
				ValidateBinaryStmt((ASTBinaryExpr*) expr);
				break;
			}
			case LITERARY: {
				ValidateLiterary((ASTLiterary*) expr);
				break;
			}
			case IDENT: {
				ValidateIdent((ASTIdent*) expr);
				break;
			}
			case FUNC_CALL: {
				ValidateFuncCall((ASTFuncCallExpr*) expr);
				break;
			}
			case ASTCAST_EXPR: {
				ValidateCast((ASTCastExpr*) expr);
				break;
			}
			case OBJECT_MEMBER_CHAIN: {
				if (!ValidateObjectChainMember((ASTObjectMemberChainExpr*) expr)){
					throw Error::UNIDENTIFIED_OBJECT_MEMBER;
				}
				break;
			}
			default: {
				Trace("Didn't process, but found", GetTokenString(expr->type));
			}
		}
	}

	/**
	 * TODO: 
	 * 		- FuncCall and func parameters should have the same type
	 */
	bool Check::ValidateObjectChainMember(ASTObjectMemberChainExpr* member){
		SetRowCol(member);

		ASTNode* node = GetObjectInScopeByString(member->object->name, scope);
		if (node != NULL){
			SetRowCol(node);
			if (node->type != OBJECT) throw Error::DIFFERENT_TYPE_ALREADY_DEFINED_IN_SCOPE;
			ASTObject* obj = (ASTObject*) node;
			std::map<std::string, ASTNode*>::const_iterator it = obj->members.find(member->member->name);
			if (it != obj->members.end()){
				SetRowCol(it->second);
				if (it->second->visibility == vPRIVATE) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
				if (it->second->type == FUNC) return ValidateMemberFuncCall((ASTFunc*) it->second, (ASTFuncCallExpr*) member->member);
				else if (it->second->type == IDENT || it->second->type == VAR) return true;
			}
		}

		std::map<std::string, ASTInclude*>::const_iterator it = file->includes.find(member->object->name);
		if (it != file->includes.end()){ // alias is faster
			Script* script = isolate->GetContext()->GetScriptByString(isolate, it->second->name);
			ASTNode* exported = script->GetExportedObject(member->member->name);
			SetRowCol(exported);
			if (exported == NULL){
				exported = script->GetExportedObject(member->object->name);
				if (exported->type != OBJECT) throw Error::UNDEFINED_OBJECT;
				
				ASTObject* obj = (ASTObject*) exported;
				SetRowCol(obj);
				std::map<std::string, ASTNode*>::const_iterator it = obj->members.find(member->member->name);
				if (it != obj->members.end()){
					SetRowCol(it->second);
					if (it->second->visibility != vPRIVATE || it->second->visibility != vPROTECTED) return true;
				}
			}
			if (exported->visibility == vPRIVATE) return false;
			if (member->member->type == FUNC_CALL && exported->type == FUNC){
				ASTFuncCallExpr* funcCall = (ASTFuncCallExpr*) member->member;
				SetRowCol(funcCall);
				ASTFunc* func = (ASTFunc*) exported;
				if (funcCall->params.size() != func->args.size()){
					throw Error::INVALID_FUNC_CALL;
				}
				return true;
			}
			else if (member->member->type == IDENT && exported->type == IDENT){
				return true;
			}
		}
		else{ // non alias
			for (int i = 0; i < file->includesList.size(); i++){
				ASTInclude* include = file->includesList[i];
				Script* script = isolate->GetContext()->GetScriptByString(isolate, include->name);
				ASTNode* exported = script->GetExportedObject(member->member->name);
				SetRowCol(exported);
				if (exported != NULL && exported->visibility != vPRIVATE) return true;
				exported = script->GetExportedObject(member->object->name);
				SetRowCol(exported);
				if (exported == NULL || exported->type != OBJECT) throw Error::UNDEFINED_OBJECT;
				ASTObject* obj = (ASTObject*) exported;
				SetRowCol(obj);
				std::map<std::string, ASTNode*>::const_iterator it = obj->members.find(member->member->name);
				if (it != obj->members.end()){
					if (it->second->visibility == vPRIVATE || it->second->visibility == vPROTECTED) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
					if (it->second->type == FUNC) return ValidateMemberFuncCall((ASTFunc*) it->second, (ASTFuncCallExpr*) member->member);
					else if (it->second->type == IDENT || it->second->type == VAR) return true;
					else return false;
				}
			}
		}
		throw Error::UNDEFINED_OBJECT;
	}

	bool Check::ValidateMemberFuncCall(ASTFunc* func, ASTFuncCallExpr* call){
		SetRowCol(call);
		if (call->params.size() != func->args.size()) return false;
		for (int i = 0; i < call->params.size(); i++){
			ValidateStmt(call->params[i]);
		}
		return true;
	}

	void Check::ValidateCast(ASTCastExpr* cast){
		SetRowCol(cast);
		ASTExpr* to = cast->to;
		if (to == NULL || to->type != LITERARY){
			throw Error::UNKNOWN_CAST_TYPE;
		}
		else{
			ASTLiterary* lit = (ASTLiterary*) to;
			SetRowCol(lit);
			cast->castType = lit->kind;
		}
		ValidateStmt(cast->value);
	}
	
	void Check::ValidateVar(ASTNode* node){
		SetRowCol(node);
		if (node->type == VAR){
			ASTVar* var = (ASTVar*) node;
			if (var->varClass != NULL && trace) Trace("\nVar type", var->varClass->name);
			else if (trace) Trace("\nValidating " + GetTokenString(var->type), node->name);
			ASTExpr* stmt = (ASTExpr*) var->stmt;
			SetRowCol(stmt);
			ValidateStmt(stmt);
			if (mode == STRICT){
				if (var->varType != stmt->assignType) throw Error::INVALID_ASSIGNMENT_TO_TYPE;
			}
			else{
				var->varType = stmt->assignType;
			}
		}
	}

	void Check::ValidateFor(ASTNode* node){
		if (trace) Trace("Validating", "For loop");
		ASTFor* forStmt = (ASTFor*) node;
		SetRowCol(forStmt);
		OpenScope(forStmt->block->scope);
		ValidateVar(forStmt->var);
		scope->Insert(forStmt->var);
		ValidateStmt(forStmt->conditions);
		ValidateStmt(forStmt->iterator);
		ScanScope();
		CloseScope();
	}

	// TODO:
	// 		Do not assum the func returned is in slot 0
	void Check::CheckScopeLevelNode(ASTNode* node){
		SetRowCol(node);
		if (node->scan){
			int type = (int) node->type;
			switch (type){
				case FUNC_CALL: {
					ValidateFuncCall((ASTFuncCallExpr*) node);
					break;
				}
				case FUNC: {
					std::vector<ASTNode*> exprs = scope->Lookup(node->name);
					if (exprs.empty()) throw Error::UNDEFINED_FUNC;
					ValidateFunc((ASTFunc*) exprs[0]);
					break;
				}
				case FOR: {
					ValidateFor(node);
					break;
				}
				case VAR: {
					ValidateVar(node);
					break;
				}
				default: {
					if (trace) Trace("Working on type...", GetTokenString(node->type));
				}
			}
		}
	}

	void Check::ScanScope(){
		if (scope->Size() > 0){
			for (int i = 0; i < scope->Size(); i++){
				CheckScopeLevelNode(scope->Get(i));
			}
		}
	}

	void Check::ValidateFuncArgs(ASTFunc* func){
		SetRowCol(func);
		if (func->args.size() > 0){
			for (int i = 0; i < func->args.size(); i++){
				ASTNode* node = func->args[i];
				SetRowCol(node);
				if (node->name.length() < 1) throw Error::EXPECTED_ARG_NAME;
				if (trace) Trace("arg[" + std::to_string(i) + "]", node->name);
				node->scan = false;
				func->body->scope->Insert(node); // add the arg to the func scope
			}
		}
	}

	void Check::ValidateFunc(ASTFunc* func){
		SetRowCol(func);
		if (func == NULL) return;
		if (trace) Trace("\nValidating func", func->name + "\n------------------------------");
		if (trace) Trace(func->name + "() total args: ", std::to_string(func->args.size()));
		ValidateFuncArgs(func);
		
		OpenScope(func->body->scope);
		CountItemsInScope();
		if (trace) Trace("\nFunc Body", "\n------------------");
		ScanScope();
		CloseScope();
	}

	bool Check::HasMain(){
		std::vector<ASTNode*> mains = scope->Lookup("main");
		bool hasMain = mains.size() == 1;
		if (trace && hasMain) Trace("Base Scope", "Has main");
		else if (trace) Trace("Base Scope", "Does not have main");
		return hasMain;
	}

	void Check::CountItemsInScope(){
		if (trace) Trace("Total items in scope", std::to_string(scope->Size()));
	}

	void Check::CloseScope(){
		scope = scope->outer;
	}

	void Check::OpenScope(Scope* s){
		scope = s;
	}

	void Check::SetScope(){
		file->scope = scope;
	}

	void Check::OpenBaseScope(){
		if (file->scope != NULL){
			if (trace) Trace("Base Scope", "Is Opened");
			scope = file->scope;
		}
		else{
			if (trace) Trace("Base Scope", "Is Null");
		}
	}
} // namespace internal
} // namespace Cobra