#include "check.h"
#include "cobra/mem/isolate.h"
#include "cobra/flags.h"

namespace Cobra{
namespace internal{

	Check* Check::New(Isolate* iso){
		Check* c = (Check*) iso->GetMemory(sizeof(Check));
		c->scope = NULL;
		c->trace = TRACE_CHECK; // debug
		c->col = 0;
		c->row = 0;
		c->printIndent = 0;
		c->file = NULL;
		c->IsInline = false;
		c->kThis = NULL;
		return c;
	}

	void Check::CheckFile(ASTFile* f){
		file = f;
		if (!IsInline) trace = false; // this one confuses me
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
			return isolate->GetContext()->GetExportedNode(isolate, name);
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
		if (sc == NULL){
			return isolate->GetContext()->GetExportedNode(isolate, ident->name);
		}
		else if (ident == NULL) return NULL;
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
					if (mode == STRICT){
						bool notIt = false;
						for (int j = 0; j < func->args.size(); j++){
							ASTParamVar* pv = (ASTParamVar*) func->args[j];
							ASTExpr* aExpr = (ASTExpr*) funcCall->params[j];
							if (pv->varType != aExpr->assignType){
								notIt = true;
								break;
							}
						}
						if (notIt) continue;
						else{
							node = func;
							break;
						}
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
		printIndent++;
		for (int i = 0; i < call->params.size(); i++){
			ValidateStmt(call->params[i]);
		}
		ASTFunc* func = (ASTFunc*) GetObjectInScope(call, scope);
		if (func == NULL) throw Error::UNDEFINED_FUNC;
		call->assignType = func->returnType;

		call->func = func;
		printIndent--;
	}

	void Check::ValidateIdent(ASTIdent* ident){
		SetRowCol(ident);
		ASTNode* ptr = GetObjectInScopeByString(ident->name, scope);
		if (ptr == NULL) throw Error::UNDEFINED_VARIABLE;
		if (ptr->type == VAR) {
			ASTVar* var = (ASTVar*) ptr;
			ident->assignType = var->varType;
			ptr->used = true;
		}
		else if (ptr->type == ASTPARAM_VAR){
			ASTParamVar* var = (ASTParamVar*) ptr;
			ident->assignType = var->varType;
			ptr->used = true;
		}
		
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

	void Check::ValidateBinaryStmt(ASTBinaryExpr* binary, bool boolean){
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
			if (boolean) {
				if (!binary->op->IsBinaryComparison()) throw Error::INVALID_BOOLEAN_COMPARISON;
			}
			SetRowCol(binary->Right);
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
			if (unary->value->type == OBJECT_INIT) unary->assignType = OBJECT_INIT;
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
				ValidateBinaryStmt((ASTBinaryExpr*) expr, false);
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
			case ARRAY_MEMBER: {
				ValidateArrayMember((ASTArrayMemberExpr*) expr);
				break;
			}
			case OBJECT_INIT: {
				ValidateObjectInit((ASTObjectInit*) expr);
				break;
			}
			default: {
				Trace("Didn't process, but found", GetTokenString(expr->type));
			}
		}
	}

	void Check::ValidateObjectInit(ASTObjectInit* init){
		if (trace) Trace("Validating new object", init->name.c_str());
		SetRowCol(init);
		ASTNode* nodeObj = GetObjectInScopeByString(init->name, scope == NULL ? NULL : scope->outer);
		if (nodeObj == NULL) throw Error::UNDEFINED_OBJECT;
		else if (nodeObj->type != OBJECT) throw Error::EXPECTED_OBJECT;
		ASTObject* obj = (ASTObject*) nodeObj;

		// Check constructor
		bool foundConstructor = false;
		for (int i = 0; i < obj->members.size(); i++){
			ASTNode* mem = obj->members[i];
			if (mem->type == FUNC && mem->name == init->name){
				ASTFunc* func = (ASTFunc*) mem;
				if (init->params.size() == func->args.size()){
					if (init->params.size() == 0){
						foundConstructor = true;
						break;
					}
					bool found = false;
					for (int j = 0; j < init->params.size(); j++){
						ValidateStmt(init->params[j]);
						if (init->params[j]->assignType != func->args[j]->varType){
							found = false;
							break;
						}
						found = true;
					}
					foundConstructor = found;
				}
			}
		}
		if (!foundConstructor) throw Error::OBJECT_CONSTRUCTOR_INVALID;
		init->assignType = OBJECT_INIT;
	}

	void Check::ValidateArrayMember(ASTArrayMemberExpr* expr){
		if (trace) Trace("Validating array member", expr->value->name.c_str());
		if (expr->value->type == IDENT) ValidateIdent((ASTIdent*) expr->value);
		ValidateIsArrayType((ASTIdent*) expr->value);
		ValidateStmt(expr->member);
	}

	/**
	* TODO:
  * 	There are more array types than a string
	**/
	void Check::ValidateIsArrayType(ASTIdent* ident){
		if (ident->value->type == STRING){}
		else if (ident->value->type == VAR || ident->value->type == ASTPARAM_VAR){
			ASTVar* var = (ASTVar*) ident->value;
			if (var->varType == STRING){
				ident->assignType = CHAR;
			}
			else throw Error::INVALID_ARRAY_TYPE;
		}
		else throw Error::INVALID_ARRAY_TYPE;
	}

	bool Check::ValidateThis(ASTObjectMemberChainExpr* member){
		ASTNode* n = kThis;
		if (n == NULL) return false;
		if (n->type == OBJECT){
			ASTObject* o = (ASTObject*) n;
			member->object->value = n;
			for (int i = 0; i < o->members.size(); i++){
				if (o->members[i]->name == member->member->name){
					if (mode == STRICT){
						if (o->members[i]->type == VAR){ 
							ASTVar* v = (ASTVar*) o->members[i];
							member->member->assignType = v->varType;
							ValidateStmt(member->value);
							SetRowCol(member->value);
							return member->value->assignType == member->member->assignType;
						}
					}
				}
			}
		}
		return false;
	}

	/**
	 * TODO: 
	 * 		- FuncCall and func parameters should have the same type
	 */
	bool Check::ValidateObjectChainMember(ASTObjectMemberChainExpr* member){
		SetRowCol(member);
		if (member->name == "%this" && member->object->name == "this"){
			if (!ValidateThis(member)) throw Error::INVALID_OBJECT_MEMBER;
			return true;
		}
		ASTNode* node = GetObjectInScopeByString(member->object->name, scope);
		if (node != NULL){
			SetRowCol(node);
			if (node->type != OBJECT) throw Error::DIFFERENT_TYPE_ALREADY_DEFINED_IN_SCOPE;
			ASTObject* obj = (ASTObject*) node;
			for (int i = 0; i < obj->members.size(); i++){
				if (obj->members[i]->name == member->member->name){
					SetRowCol(obj->members[i]);
					if (obj->members[i]->visibility == vPRIVATE) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
					if (obj->members[i]->type == FUNC) {
						ASTFunc* f = (ASTFunc*) obj->members[i];
						ASTFuncCallExpr* fc = (ASTFuncCallExpr*) member->member;
						if (ValidateMemberFuncCall(f, fc)){
							member->assignType = f->returnType;
							if (member->name.find('%') != std::string::npos){
								if (f->visibility != vSTATIC) throw Error::UNABLE_TO_ACCESS_NON_STATIC_FUNCTION;
							}
							return true;
						}
					}
					else if (obj->members[i]->type == IDENT || obj->members[i]->type == VAR) return true;
				}
			}
		}

		std::map<std::string, ASTInclude*>::const_iterator it = file->includes.find(member->object->name);
		if (it != file->includes.end()){ // alias is faster
			Script* script = isolate->GetContext()->GetScriptByString(isolate, it->second->name);
			ASTNode* exported = script->GetExportedObject(member->member->name);
			SetRowCol(exported);
			if (exported == NULL){
				//printf("d %s\n", member->object->name.c_str());
				exported = script->GetExportedObject(member->name);
				if (exported == NULL || exported->type != OBJECT) throw Error::UNDEFINED_OBJECT;
				
				ASTObject* obj = (ASTObject*) exported;
				SetRowCol(obj);
				for (int i = 0; i < obj->members.size(); i++){
					if (obj->members[i]->name == member->member->name){
						SetRowCol(obj->members[i]);
						if (obj->members[i]->visibility == vPRIVATE) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
						if (obj->members[i]->type == FUNC) return ValidateMemberFuncCall((ASTFunc*) obj->members[i], (ASTFuncCallExpr*) member->member);
						else if (obj->members[i]->type == IDENT || obj->members[i]->type == VAR) return true;
					}
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
				for (int i = 0; i < obj->members.size(); i++){
					if (obj->members[i]->name == member->member->name){
						SetRowCol(obj->members[i]);
						if (obj->members[i]->visibility == vPRIVATE) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
						if (obj->members[i]->type == FUNC) return ValidateMemberFuncCall((ASTFunc*) obj->members[i], (ASTFuncCallExpr*) member->member);
						else if (obj->members[i]->type == IDENT || obj->members[i]->type == VAR) return true;
					}
				}
			}
		}
		throw Error::UNDEFINED_OBJECT;
	}

	bool Check::ValidateMemberFuncCall(ASTFunc* func, ASTFuncCallExpr* call){
		SetRowCol(call);
		if (func->returnType == ILLEGAL) ValidateFunc(func);
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
			cast->assignType = cast->castType;
		}
		ValidateStmt(cast->value);
	}
	
	void Check::ValidateVar(ASTNode* node){
		SetRowCol(node);
		if (node->type == VAR){
			ASTVar* var = (ASTVar*) node;
			if (var->varClass != NULL && trace) Trace("\nVar type", var->varClass->name);
			else if (trace) Trace("\nValidating " + GetTokenString(var->type), node->name);
			ASTExpr* stmt = var->stmt;
			SetRowCol(stmt);
			if (stmt != NULL){
				ValidateStmt(stmt);
				if (mode == STRICT){
					if (var->varClass != NULL && stmt->assignType == OBJECT_INIT){
						var->varType = OBJECT_INIT;
					}
					else if (var->varType != stmt->assignType && var->name != "return") {
						SetRowCol(var);
						throw Error::INVALID_ASSIGNMENT_TO_TYPE;
					}
				}
				else{
					var->varType = stmt->assignType;
				}
			}
			else{
				stmt = ASTNull::New(isolate);
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

	void Check::ValidateObject(ASTObject* obj){
		if (trace) Trace("Validating object", obj->name);
		SetRowCol(obj);
		kThis = obj;
		for (int i = 0; i < obj->members.size(); i++){
			ASTNode* mem = obj->members[i];
			if (mem != NULL) CheckScopeLevelNode(mem);
		}
	}

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
					ValidateFunc((ASTFunc*) node);
					break;
				}
				case OBJECT: {
					ValidateObject((ASTObject*) node);
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
				case IF: {
					ValidateIf((ASTIf*) node);
					break;
				}
				case TRY_CATCH: {
					ValidateTryCatch((ASTTryCatch*) node);
					break;
				}
				case OBJECT_MEMBER_CHAIN: {
					if (!ValidateObjectChainMember((ASTObjectMemberChainExpr*) node)){
						throw Error::UNIDENTIFIED_OBJECT_MEMBER;
					}
					break;
				}
				case THROW: {
					ValidateThrow((ASTThrow*) node);
					break;
				}
				case IDENT: {
					break;
				}
				default: {
					if (trace) Trace("Working on type...", GetTokenString(node->type));
				}
			}
		}
	}

	void Check::ValidateThrow(ASTThrow* expr){
		SetRowCol(expr);
		if (expr->obj == NULL) throw Error::INVALID_THROW;
		SetRowCol(expr->obj);
		if (expr->obj->type == IDENT) ValidateIdent((ASTIdent*) expr->obj);
		else if (expr->obj->type == OBJECT_INIT) ValidateObjectInit((ASTObjectInit*) expr->obj);
	}

	void Check::ValidateTryCatch(ASTTryCatch* try_catch){
		OpenScope(try_catch->tryBlock->scope);
		CountItemsInScope();
		if (trace) Trace("\nTry_Catch Body", "\n------------------");
		ScanScope();
		CloseScope();
		ValidateFunc(try_catch->catchFunc);
	}

	void Check::ValidateIf(ASTIf* ifStmt){
		SetRowCol(ifStmt);
		if (trace) Trace("Validating", "if statement");
		if (ifStmt->conditions->type == BINARY) ValidateBinaryStmt((ASTBinaryExpr*) ifStmt->conditions, true);
		else throw Error::INVALID_CONDITION_FOR_IF_STMT;

		OpenScope(ifStmt->block->scope);
		ScanScope();
		CloseScope();
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
				ASTParamVar* node = func->args[i];
				if (node->varClass != NULL) ValidateIdent(node->varClass);
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
		
		if (func->body->scope == NULL) return;
		OpenScope(func->body->scope);
		CountItemsInScope();
		if (trace) Trace("\nFunc Body", "\n------------------");
		ScanScope();
		std::vector<ASTNode*> returnVars = func->body->scope->Lookup("return");
		if (returnVars.size() > 0){
			ASTVar* returnVar = (ASTVar*) returnVars[0];
			if (returnVar->stmt->type == ASTCAST_EXPR){
				ASTCastExpr* cast = (ASTCastExpr*) returnVar->stmt;
				func->returnType = cast->castType;
			}
			else if (returnVar->stmt->type == BINARY){
				func->returnType = BOOLEAN;
			}
			else{
				func->returnType = returnVar->stmt->assignType;
			}
		}
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