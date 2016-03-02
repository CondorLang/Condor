	#include "check.h"
#include "cobra/mem/isolate.h"
#include "cobra/flags.h"

namespace Cobra{
namespace internal{

	Check* Check::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(Check));
		Check* c = new(p) Check();
		c->scope = NULL;
		c->trace = TRACE_CHECK; // debug
		c->col = 0;
		c->row = 0;
		c->printIndent = 0;
		c->file = NULL;
		c->IsInline = false;
		c->kThis = NULL;
		c->kPreviousObject = NULL;
		c->expand = EXPAND_AST;
		c->expandIndent = 0;
		return c;
	}

	void Check::CheckFile(ASTFile* f){
		file = f;
		if (!IsInline) trace = false;
		if (expand) Expand("File", "");
		OpenBaseScope();
		CountItemsInScope();
		ScanScope();
		SetScope();
	}

	void Check::Expand(std::string msg1, std::string msg2){
		std::string out;
		for (int i = 0; i < expandIndent; i++){
			out += "  ";
		}
		out += msg1 + ": " + msg2;
		printf("%s\n", out.c_str());
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
							ASTVar* pv = (ASTVar*) func->args[j];
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
						if (nodes[i]->type == VAR || nodes[i]->type == VAR){
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
		if (expand) Expand("Func Call", call->name);
		printIndent++;
		expandIndent++;
		if (expand) Expand("Params", "");
		expandIndent++;
		for (int i = 0; i < call->params.size(); i++){
			ValidateStmt(call->params[i]);
		}
		ASTFunc* func = (ASTFunc*) GetObjectInScope(call, scope);
		if (func == NULL) throw Error::UNDEFINED_FUNC;
		call->assignType = func->returnType;

		call->func = func;
		printIndent--;
		expandIndent--;
		expandIndent--;
	}

	void Check::ValidateIdent(ASTIdent* ident){
		SetRowCol(ident);
		ASTNode* ptr = GetObjectInScopeByString(ident->name, scope);
		if (ptr == NULL){
			// double check kThis
			if (kThis != NULL && kThis->type == OBJECT){
				ASTObject* o = (ASTObject*) kThis;
				for (int i = 0; i < o->members.size(); i++){
					if (ident->name == o->members[i]->name){
						ptr = o->members[i];
					}
				}
			}
			if (ptr == NULL) throw Error::UNDEFINED_VARIABLE;
		}
		if (ptr->type == VAR) {
			ASTVar* var = (ASTVar*) ptr;
			ident->assignType = var->varType;
			ptr->used = true;
		}
		else if (ptr->type == VAR){
			ASTVar* var = (ASTVar*) ptr;
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

	/**
	 * Maybe we don't do anything during the check
	 */
	void Check::ValidateNot(ASTBinaryExpr* binary){

	}

	void Check::ValidateBinaryStmt(ASTBinaryExpr* binary, bool boolean){
		SetRowCol(binary);
		if (binary == NULL){
			if (trace) Trace("Binary", "is NULL");
		}
		else if (binary->op == NULL){
			throw Error::INVALID_OPERATOR;
		}
		else{
			if (binary->Left != NULL) ValidateStmt(binary->Left);
			if (trace) Trace("Operator found", binary->op->String());
			if (binary->Right != NULL) ValidateStmt(binary->Right);
			else throw Error::INVALID_BOOLEAN_COMPARISON;
			if (binary->Left == NULL){
				if (binary->op->value == NOT){
					return ValidateNot(binary);
				}
				else{
					throw Error::INVALID_BOOLEAN_COMPARISON;
				}
			}
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
			int type = (int) unary->value->type;
			switch (type){
				case OBJECT_INIT: {
					unary->assignType = OBJECT_INIT;
					break;
				}
				case OBJECT_MEMBER_CHAIN: {
					ASTObjectMemberChainExpr* mem = (ASTObjectMemberChainExpr*) unary->value;
					unary->assignType = mem->assignType;
					break;
				}
				case ASTCAST_EXPR: {
					ASTCastExpr* cast = (ASTCastExpr*) unary->value;
					unary->assignType = cast->castType;
					break;
				}
				case FUNC_CALL: {
					ASTFuncCallExpr* funcCall = (ASTFuncCallExpr*) unary->value;
					unary->assignType = funcCall->func->returnType;
					break;
				}
			}
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
				ValidateObjectChainMember((ASTObjectMemberChainExpr*) expr);
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
			case AST_NOT: {
				ValidateAstNot((ASTNot*) expr);
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
					break;
				}
			}
		}
		if (!foundConstructor) throw Error::OBJECT_CONSTRUCTOR_INVALID;
		init->assignType = OBJECT_INIT;
		init->base = obj;
		if (expand){
			expandIndent++;
			expandIndent++;
			for (int i = 0; i < init->params.size(); i++){
				Expand("arg[" + std::to_string(i) + "]", GetTokenString(init->params[i]->assignType));
			}
			expandIndent--;
			expandIndent--;
		}
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
		else if (ident->value->type == VAR){
			ASTVar* var = (ASTVar*) ident->value;
			if (var->varType == STRING){
				ident->assignType = CHAR;
			}
			else throw Error::INVALID_ARRAY_TYPE;
		}
		else throw Error::INVALID_ARRAY_TYPE;
	}

	/**
	 * this.base
	 * this.base.length;
	 * this.base.base.length; 
	 */
	void Check::ValidateObjectChainMember(ASTObjectMemberChainExpr* member){
		SetRowCol(member);
		ASTNode* node = NULL;
		if (member->name == "this"){
			member->object = kThis;
		}
		else{
			node = member->object;
		}
		std::string lookupValue = member->member->name;

		if (member->member->type == BINARY){
			ASTBinaryExpr* binary = (ASTBinaryExpr*) member->member;
			lookupValue = 	binary->Left->name;
		}
		if (lookupValue.empty()){
			throw Error::EXPECTED_OBJECT_MEMBER_NAME;
		}
		ASTObject* obj = NULL;
		if (member->object->type == IDENT){
			ASTIdent* ident = (ASTIdent*) member->object;
			if (ident->value == NULL){
				ASTNode* value = GetObjectInScopeByString(ident->name, scope);
				if (value == NULL) {	
					throw Error::UNDEFINED_OBJECT;
				}
				ident->value = value;
				if (value->type == OBJECT) obj = (ASTObject*) value;
			}
		}
		if (obj == NULL){
			if (member->object->type == OBJECT) obj = (ASTObject*) member->object;
			else if (member->object->type == IDENT){
				ASTIdent* ident = (ASTIdent*) member->object;
				if (ident->value->type == VAR){
					ASTVar* var = (ASTVar*) ident->value;
					obj = (ASTObject*) var->varClass;
				}
			}
		}
		if (obj == NULL) throw Error::UNDEFINED_OBJECT;
		if (obj->type == IDENT){
			ASTNode* n = GetObjectInScopeByString(obj->name, scope);
			if (n->type == OBJECT) obj = (ASTObject*) n;	
		}
		bool found = false;
		bool cont = false;
		if (obj->members.empty()) throw Error::INVALID_OBJECT_MEMBER;
		for (int i = 0; i < obj->members.size(); i++){
			if (obj->members[i] == NULL) throw Error::CORRUPT_OBJECT;
			if (obj->members[i]->name == lookupValue){
				ASTNode* target = obj->members[i];
				int type = (int) target->type;
				switch (type){
					case VAR: case ARRAY:{
						ASTVar* var = (ASTVar*) obj->members[i];
						if (var->varClass == NULL || type == ARRAY) SetVarClass(var, true);
						if (type == ARRAY){
							int a = 10;
						}
						if (member->member->type == OBJECT_MEMBER_CHAIN){
							ASTObjectMemberChainExpr* mem = (ASTObjectMemberChainExpr*) member->member;
							mem->object = var->varClass;
						}
						member->assignType = var->varType;
						found = true;
						break;
					}
					case IDENT: {
						if (obj->members[i]->type == VAR){
							ASTVar* var = (ASTVar*) obj->members[i];
							member->member->assignType = var->varType;
							ValidateStmt(member->value);
							found = var->varType == member->member->assignType;
							if (found){
								if (member->member->value == NULL) SetVarClass(var, false);
								member->assignType = var->varType;
								break;
							}
						}
						else if (obj->members[i]->type == FUNC){
							ASTFunc* func = (ASTFunc*) obj->members[i];
							member->member->assignType = func->returnType;
							member->assignType = func->returnType;
							found = true;
							break;
						}
						break;
					}
					case ARRAY_MEMBER: {
						ASTArrayMemberExpr* ary = (ASTArrayMemberExpr*) member->member;
						ValidateStmt(ary->member);
						ValidateStmt(ary->value);
						member->assignType = ary->value->assignType;
						found = true;
						break;
					}
					case FUNC: {
						ASTFuncCallExpr* call = (ASTFuncCallExpr*) member->member;
						if (call->type == BINARY) { // bug fixing the !str2.isSomething()
							ASTBinaryExpr* binary = (ASTBinaryExpr*) call;
							call = (ASTFuncCallExpr*) binary->Left;
						}
						ASTFunc* func = (ASTFunc*) obj->members[i];
						if (func->args.size() == call->params.size()){
							bool foundFunc = false;
							if (func->args.size() == 0) foundFunc = true;
							else{
								for (int j = 0; j < func->args.size(); j++){
									ValidateStmt(call->params[j]);
									if (func->args[j]->varType == call->params[j]->assignType){
										foundFunc = true;
									}
									else{
										foundFunc = false;
										cont = true;
										break;
									}
								}
							}
							if (foundFunc){
								call->func = func;
								call->assignType = func->returnType;
								member->assignType = call->assignType;
								found = true;
								break;
							}
						}
						break;
					}
					case OBJECT_MEMBER_CHAIN: {
						ASTObjectMemberChainExpr* chain = (ASTObjectMemberChainExpr*) member->member;
						ValidateObjectChainMember(chain);
						member->assignType = chain->assignType;
						found = true;
						break;
					}
				}
			}
			if (member->member->type == OBJECT_MEMBER_CHAIN && found){
				ASTObjectMemberChainExpr* chain = (ASTObjectMemberChainExpr*) member->member;
				ValidateObjectChainMember(chain);
				member->assignType = chain->assignType;
			}
			if (found) break;
		}
		if (!found)	throw Error::INVALID_OBJECT_MEMBER;
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
			if (expand && var->varClass != NULL) Expand("Var", "Name: '" + var->name + "', Object Type: " + var->varClass->name);
			else if (expand) Expand("Var", "Name: '" + var->name + "', Type: " + GetTokenString(var->type));
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
						ASTObjectInit* init = CallDefaultConstructor(var, stmt);
						VerifyTypeConversion(var);
						if (init != NULL) var->stmt = init;
						else if (var->varType == var->stmt->assignType){}
						else {
							throw Error::INVALID_ASSIGNMENT_TO_TYPE;
						}
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

	void Check::VerifyTypeConversion(ASTVar* var){
		if (var->stmt == NULL) return;
		if (var->varType != var->stmt->assignType){
			Token* vt = Token::New(isolate, var->varType);
			Token* st = Token::New(isolate, var->stmt->assignType);
			if ((vt->IsNumber() && st->IsNumber()) || (vt->IsString() && st->IsString())){
				var->stmt->assignType = var->varType;
			}
		}
	}

	ASTObjectInit* Check::CallDefaultConstructor(ASTVar* var, ASTExpr* stmt){
		if (var->varClass == NULL) return NULL;
		ASTObjectInit* call = ASTObjectInit::New(isolate);
		call->name = var->varClass->name;
		call->row = var->row;
		call->col = var->col;
		call->params.push_back(stmt);
		expandIndent++;
		if (expand) Expand("Func: Default Constructor", call->name);
		expandIndent--;
		ValidateObjectInit(call);
		return call;
	}

	void Check::ValidateFor(ASTNode* node){
		if (trace) Trace("Validating", "For loop");
		if (expand) Expand("For Loop", "");
		ASTFor* forStmt = (ASTFor*) node;
		SetRowCol(forStmt);
		OpenScope(forStmt->block->scope);
		expandIndent++;
		ValidateVar(forStmt->var);
		if (expand) Expand("Variable", forStmt->var->name);
		scope->Insert(forStmt->var);
		ValidateStmt(forStmt->conditions);
		ValidateStmt(forStmt->iterator);
		expandIndent--;
		ScanScope();
		CloseScope();
	}

	void Check::ValidateObject(ASTObject* obj){
		if (trace) Trace("Validating object", obj->name);
		SetRowCol(obj);
		if (obj->name[0] == '$'){
			if (trace) Trace("Extending object", obj->name.substr(1).c_str());
			if (expand) Expand("Object extension", obj->name.substr(1));
			ASTNode* ext = GetObjectInScopeByString(obj->name.substr(1), scope);
			if (ext == NULL) throw Error::INVALID_OBJECT_EXTENSION;
			if (ext->type == OBJECT){
				ASTObject* objToAddTo = (ASTObject*) ext;
				expandIndent++;
				for (int i = 0; i < obj->members.size(); i++){
					if (expand) Expand("member[" + std::to_string(i) + "]", obj->members[i]->name);
					objToAddTo->members.push_back(obj->members[i]);
				}
				expandIndent--;
				isolate->FreeMemory(obj, sizeof(ASTObject)); // destroy the extended object
				obj = objToAddTo;
			}
		}
		kThis = obj;
		if (expand) Expand("Object", obj->name);
		for (int i = 0; i < obj->members.size(); i++){
			ASTNode* mem = obj->members[i];
			expandIndent++;
			if (expand) Expand("member[" + std::to_string(i) + "]", mem->name);
			if (mem != NULL) CheckScopeLevelNode(mem);
			if (mem->type == VAR){
				ASTVar* var = (ASTVar*) mem;
				ASTUnaryExpr* unary = (ASTUnaryExpr*) var->stmt;
				if (unary == NULL){
					unary = ASTUnaryExpr::New(isolate);
					unary->op = Token::New(isolate, ASSIGN);
					var->stmt = unary;
				}
				if (unary->value == NULL) {
					unary->value = ASTUndefined::New(isolate);
					int a = 10;
				}
				SetVarClass(var, false);
			}
			expandIndent--;
		}
	}

	void Check::SetVarClass(ASTVar* var, bool force){
		if (var->varClass == NULL || force){
			int type = (int) var->varType;
			switch (type){
				case STRING: {
					var->varClass = GetObjectInScopeByString("String", scope);
					break;
				}
				case ARRAY: {
					var->varClass = GetObjectInScopeByString("Array", scope);
					break;
				}
				default: {
					var->varClass = ASTUndefined::New(isolate);
					break;
				}
			}
		}
	}

	void Check::ValidateArray(ASTArray* array){
		if (trace) Trace("Validating array", array->name);
		if (expand) Expand("Array", array->name);
		SetRowCol(array);
		if (array->arrayType == IDENT){
			if (array->varClass == NULL) throw Error::EXPECTED_ARRAY_TYPE;
		}
	}
	//here
	void Check::CheckScopeLevelNode(ASTNode* node){
		SetRowCol(node);
		if (node == NULL) return;
		expandIndent++;
		if (node->scan){
			int type = (int) node->type;
			switch (type){
				case ARRAY: {
					ValidateArray((ASTArray*) node);
					break;
				}
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
					ValidateObjectChainMember((ASTObjectMemberChainExpr*) node);
					break;
				}
				case THROW: {
					ValidateThrow((ASTThrow*) node);
					break;
				}
				case IDENT: {
					ValidateIdent((ASTIdent*) node);
					break;
				}
				default: {
					if (trace) Trace("Working on type...", GetTokenString(node->type));
				}
			}
		}
		expandIndent--;
	}

	void Check::ValidateThrow(ASTThrow* expr){
		SetRowCol(expr);
		if (expr->obj == NULL) throw Error::INVALID_THROW;
		SetRowCol(expr->obj);
		if (expand) Expand("Throw", expr->obj->name);
		if (expr->obj->type == IDENT) ValidateIdent((ASTIdent*) expr->obj);
		else if (expr->obj->type == OBJECT_INIT) ValidateObjectInit((ASTObjectInit*) expr->obj);
	}

	void Check::ValidateTryCatch(ASTTryCatch* try_catch){
		OpenScope(try_catch->tryBlock->scope);
		CountItemsInScope();
		if (trace) Trace("\nTry_Catch Body", "\n------------------");
		if (expand) Expand("Try Catch", "");
		ScanScope();
		CloseScope();
		ValidateFunc(try_catch->catchFunc);
	}

	void Check::ValidateAstNot(ASTNot* astNot){
		if (trace) Trace("Validating", "not statement");
		ValidateStmt(astNot->value);
		// here
	}

	void Check::ValidateIf(ASTIf* ifStmt){
		SetRowCol(ifStmt);
		if (trace) Trace("Validating", "if statement");
		if (expand) Expand("If", "");
		if (ifStmt->conditions->type == BINARY) ValidateBinaryStmt((ASTBinaryExpr*) ifStmt->conditions, true);
		else if (ifStmt->conditions->type == AST_NOT) ValidateAstNot((ASTNot*) ifStmt->conditions);
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
		if (func == NULL) return;
		SetRowCol(func);
		expandIndent++;
		if (func->args.size() > 0){
			for (int i = 0; i < func->args.size(); i++){
				ASTVar* node = func->args[i];
				if (node->varClass != NULL && node->varClass->type == IDENT) ValidateIdent((ASTIdent*) node->varClass);
				SetRowCol(node);
				if (node->name.length() < 1) throw Error::EXPECTED_ARG_NAME;
				if (trace) Trace("arg[" + std::to_string(i) + "]", node->name);
				if (expand) Expand("arg[" + std::to_string(i) + "]", node->name);
				node->scan = false;
				func->body->scope->Insert(node); // add the arg to the func scope
			}
		}
		expandIndent--;
	}

	void Check::ValidateFunc(ASTFunc* func){
		SetRowCol(func);
		if (func == NULL) return;
		if (trace) Trace("\nValidating func", func->name + "\n------------------------------");
		if (trace) Trace(func->name + "() total args: ", std::to_string(func->args.size()));
		if (expand) Expand("Func", func->name);
		expandIndent++;
		if (expand) Expand("Args", "");
		ValidateFuncArgs(func);
		
		if (func->body->scope == NULL) return;
		OpenScope(func->body->scope);
		CountItemsInScope();
		if (trace) Trace("\nFunc Body", "\n------------------");
		if (expand) Expand("Body", "");
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
			if (expand) {
				Token* t = Token::New(isolate, returnVar->stmt->assignType);
				Expand("Return Type", t->String());
				isolate->FreeMemory(t, sizeof(Token));
			}
		}
		CloseScope();
		expandIndent--;
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
			expandIndent++;
			if (trace) Trace("Base Scope", "Is Opened");
			if (expand) Expand("Scope", "Base");
			scope = file->scope;
		}
		else{
			if (trace) Trace("Base Scope", "Is Null");
		}
	}
} // namespace internal
} // namespace Cobra