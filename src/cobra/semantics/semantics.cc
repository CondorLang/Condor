#include "semantics.h"

namespace Cobra {
namespace internal{

	Semantics::Semantics(Isolate* iso){
		isolate = iso;
		row = -1;
		col = -1;
		baseScope = NULL;
		trace = TRACE_SEMANTIC;
		indent = 0;
		inObject = false;
		kThis = NULL;
		isThis = false;
		staticRequired = false;
		expand = EXPAND_AST;
	}

	Semantics::~Semantics(){}

	Semantics* Semantics::New(Isolate* iso, Parser* parser){
		void* p = (Semantics*) iso->GetMemory(sizeof(Semantics));
		Semantics* n = new(p) Semantics(iso);
		n->parser = parser;
		if (parser->IsInternal()){
			n->trace = false;
		}
		return n;
	}

	void Semantics::SetRowCol(ASTNode* node){
		if (node == NULL) return;
		row = node->row;
		col = node->col;
	}

	std::string* Semantics::GetSource(){
		Scope* s = GetCurrentScope();
		if (s == baseScope) {
			return parser->GetSource();
		}
		return &s->raw;
	}

	void Semantics::Trace(const char* first, const char* second){
		if (trace) {
			std::string tabs = "";
			for (int i = 0; i < indent; i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first, second);
		}
	}

	void Semantics::Trace(const char* first, TOKEN second){
		if (trace) {
			Token* t = Token::New(isolate, second);
			std::string tabs = "";
			for (int i = 0; i < indent; i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first, t->String().c_str());
		}
	}

	Scope* Semantics::Parse(Scope* scope){
		try {
			return Parser::Parse(isolate, scope, this);
		}
		catch (Error::ERROR e){
			OpenScope(scope);
			throw e;
		}
	}

	Scope* Semantics::GetCurrentScope(){
		if (scopes.size() == 0) return NULL;
		return scopes[0];
	}

	Scope* Semantics::GetPreviousScope(){
		if (scopes.size() < 2) return NULL;
		return scopes[1];
	}

	void Semantics::SwapScopes(){
		if (scopes.size() < 2) throw Error::INTERNAL_SCOPE_ERROR;
		Scope* s = scopes[0];
		Scope* s2 = scopes[1];
		CloseScope();
		CloseScope();
		OpenScope(s);
		OpenScope(s2);
	}
	
	void Semantics::Evaluate(Scope* scope){
		if (baseScope == NULL) baseScope = scope;
		ScanScope(scope);
	}

	void Semantics::ScanScope(Scope* scope){
		Trace("Scanning Scope", (std::to_string(scope->Size()) + " items").c_str());
		indent++;
		if (scope->outer == NULL) scope->outer = GetCurrentScope();
		OpenScope(scope);
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			SetRowCol(node);
			int type = (int) node->type;
			switch (type){
				case VAR: ValidateVar((ASTVar*) node); break;
				case FOR: ValidateFor((ASTForExpr*) node); break;
				case WHILE: ValidateWhile((ASTWhileExpr*) node); break;
				case IF: ValidateIf((ASTIf*) node); break;
				case SWITCH: ValidateSwitch((ASTSwitch*) node); break;
				case FUNC: {
					ASTFunc* func = (ASTFunc*) node;
					func->scope->outer = scope;
					break;
				}
				case OBJECT: {
					ASTObject* obj = (ASTObject*) node;
					obj->scope->outer = scope;
					break;
				}
				default: ValidateExpr((ASTExpr*) node); break;
			}
		}
		indent--;
		CloseScope();
	}

	void Semantics::ValidateCase(ASTCase* expr){
		Trace("Validating", "Case");
		SetRowCol(expr);
		if (expr->isDefault && expr->condition != NULL) throw Error::INVALID_DEFAULT_STMT;
		else if (expr->condition != NULL && expr->condition->type != LITERAL) throw Error::INVALID_CASE_STMT;
		ValidateExpr((ASTExpr*) expr->condition);
	}

	void Semantics::ValidateSwitch(ASTSwitch* expr){
		Trace("Validating", "Switch");
		SetRowCol(expr);
		ValidateExpr((ASTExpr*) expr->value);
		for (int i = 0; i < expr->cases.size(); i++){
			ValidateCase(expr->cases[i]);
		}
	}

	void Semantics::ValidateVar(ASTVar* var){
		SetRowCol(var);
		var->scopeId = GetCurrentScope()->scopeId;
		if (var->isObject && var->value->type == FUNC_CALL){
			ValidateObjectInit(var);
			return;
		}
		Trace("Validating Var", var->name.c_str());
		indent++;
		if (var->name == "month"){
			int a = 10; // here
		}
		var->assignmentType = ValidateExpr((ASTExpr*) var->value);
		if (var->name != "return" &&
			  var->baseType == UNDEFINED &&
			  (var->assignmentType == UNDEFINED ||
			   var->assignmentType == kNULL)) {
			throw Error::INVALID_ASSIGNMENT_TO_TYPE;
		}
		if (var->assignmentType == UNDEFINED) Trace("Type", var->baseType);
		else Trace("Type", var->assignmentType);
		if (var->isArray) Trace("Array", "True");
		if (var->isArray && var->member != NULL){
			ASTArray* ary = (ASTArray*) var->member;
			if (ary->members.size() == 0 || ary->members.size() > 1) throw Error::INVALID_ACCESS_TO_ARRAY;
			ValidateExpr(ary->members[0]);
			ExpectNumber((ASTLiteral*) ary->members[0]);
		}
		ValidateBaseAndAssignment(var);
		if (var->previouslyDeclared){
			ASTLiteral* member = (ASTLiteral*) var->member;
			ValidateLiteral(member);
			var->member = member->var;
		}
		if (var->baseType == VAR && var->assignmentType == ILLEGAL) {
			var->assignmentType = OBJECT;
			var->baseName = GetBaseName((ASTExpr*) var->value);
		}
		indent--;
	}

	std::string Semantics::GetBaseName(ASTExpr* expr){
		if (expr == NULL) return NULL;
		int type = (int) expr->type;
		switch (type){
			case FUNC_CALL: {
				ASTFuncCall* call = (ASTFuncCall*) expr;
				std::vector<ASTNode*> nodes = call->func->scope->Lookup("return", false);
				if (nodes.size() == 0) return "";
				ASTVar* returnVar = (ASTVar*) nodes[0];
				return returnVar->baseName;
			}
			default: {
				printf("ddd: %s\n", Token::ToString(expr->type).c_str());
			}
		}
		return "";
	}

	TOKEN Semantics::ValidateExpr(ASTExpr* expr){
		SetRowCol(expr);
		if (expr == NULL) return UNDEFINED;
		if (expr->cast != NULL) ValidateCast(expr);
		int type = (int) expr->type;
		switch (type){
			case UNDEFINED: return UNDEFINED;
			case BINARY: return ValidateBinary((ASTBinaryExpr*) expr);
			case LITERAL: return ValidateLiteral((ASTLiteral*) expr);
			case FUNC_CALL: return ValidateFuncCall((ASTFuncCall*) expr);
			case ARRAY: return ValidateArray((ASTArray*) expr);
			case BREAK: return UNDEFINED;
			default: {
				throw Error::NOT_IMPLEMENTED;
			}
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateLiteral(ASTLiteral* lit){
		SetRowCol(lit);
		Trace("Literal Value", lit->value.c_str());
		int type = (int) lit->litType;
		TOKEN result = UNDEFINED;
		switch (type){
			case IDENT: result = ValidateIdent(lit); break;
		}
		ValidateExpr(lit->member);
		if (result == UNDEFINED) return lit->litType;
		return result;
	}

	void Semantics::ValidateFor(ASTForExpr* expr){
		SetRowCol(expr);
		Trace("Validating", "For");
		indent++;
		ValidateBoolean((ASTBinaryExpr*) expr->condition);
		expr->scope = Parse(expr->scope);
		expr->scope->InsertBefore(expr->tick);
		expr->scope->InsertBefore(expr->condition);
		expr->scope->InsertBefore(expr->var);
		ScanScope(expr->scope);
		expr->scope->Remove(expr->tick);
		expr->scope->Remove(expr->condition);
		indent--;
	}

	void Semantics::ValidateBoolean(ASTBinaryExpr* expr){
		SetRowCol(expr);
		if (!expr->isBoolean) throw Error::INVALID_BOOLEAN_COMPARISON;
	}

	void Semantics::ValidateIf(ASTIf* expr){
		if (expr == NULL) return;
		SetRowCol(expr);
		Trace("Validating", "If");
		if (!expr->isElse){
			ValidateExpr(expr->condition);
			ValidateBoolean((ASTBinaryExpr*) expr->condition);
		}
		indent++;
		expr->scope = Parse(expr->scope);
		ScanScope(expr->scope);
		indent--;
		for (int i = 0; i < expr->elseIfs.size(); i++) ValidateIf(expr->elseIfs[i]);
	}

	TOKEN Semantics::ValidateBinary(ASTBinaryExpr* expr){
		if (expr->op == PERIOD) return ValidateObjectChain(expr);;
		SetRowCol(expr);
		TOKEN left = ValidateExpr(expr->left);
		isThis = false; // TODO: Verify this is ok and always true

		staticRequired = false;
		if (expr->left->type == OBJECT) staticRequired = true;

		bool working = inObject; // used for object chains
		if (inObject){
			SwapScopes();
			inObject = false;
		}

		TOKEN right = ValidateExpr(expr->right);

		if (working){
			SwapScopes();
			kThis = NULL;
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateIdent(ASTLiteral* expr){
		SetRowCol(expr);
		expr->name = expr->value;
		Trace("Validating Ident", expr->name.c_str());
		Scope* s = GetCurrentScope();
		if (expr->name == "this"){ // here
			if (s->outer == NULL) throw Error::INVALID_USAGE_OF_THIS;
			kThis = (ASTObject*) s->outer->owner;
			//OpenScope(s->outer);
			if (kThis == NULL) kThis = (ASTObject*) s->owner;
			if (kThis == NULL) throw Error::INVALID_USAGE_OF_THIS;
			isThis = true;
			return UNDEFINED;
		}	
		std::vector<ASTNode*> nodes = s->Lookup(expr->name, !isThis);
		if (nodes.size() == 1 && nodes[0] == expr) throw Error::UNDEFINED_VARIABLE;
		if (nodes.size() == 0 && scopes.size() > 1) {
			SwapScopes(); // used in object chains
			s = GetCurrentScope();
			nodes = s->Lookup(expr->name, !isThis);
			SwapScopes();
			if (nodes.size() == 0 || (nodes.size() == 1 && nodes[0] == expr)) {
				SetRowCol(expr);
				throw Error::UNDEFINED_VARIABLE; 
			}
		}
		for (int i = 0; i < nodes.size(); i++){
			if ((!isThis && !expr->allowAccess) && nodes[i]->HasVisibility(PRIVATE)) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
			if ((nodes[i]->type == VAR || nodes[i]->type == OBJECT)) {
				expr->var = (ASTVar*) nodes[i];
				Scope* s = GetCurrentScope();
				expr->allowGC = s->scopeId == expr->var->scopeId;
				break;
			}
		}

		if (expr->var == NULL) {
			throw Error::UNDEFINED_VARIABLE;
		}
		if (expr->var->assignmentType == UNDEFINED) return expr->var->baseType;
		return expr->var->assignmentType;
	}

	TOKEN Semantics::ValidateFuncCall(ASTFuncCall* expr, bool isConstructor){
		if (!isConstructor) isConstructor = expr->isInit;
		Trace("Validating Func Call", expr->name.c_str());
		SetRowCol(expr);
		if (expr->isInternal) return ValidateInternal(expr);
		int funcs = 0;
		std::vector<ASTNode*> nodes = GetCurrentScope()->Lookup(expr->name);
		if (isConstructor){
			for (int i = 0; i < nodes.size(); i++){
				if (nodes[i]->type == OBJECT){
					ASTObject* obj = (ASTObject*) nodes[i];
					if (!obj->scope->IsParsed()) obj->scope = Parse(obj->scope);
					nodes = obj->scope->Lookup(expr->name, false);
					break;
				}
			}
		}

		if (nodes.size() == 0) throw Error::UNDEFINED_FUNC;
		for (int i = 0; i < nodes.size(); i++){ // count functions, if funcs > 1, then worry about the parameters
			if (nodes[i]->type == FUNC) funcs++;
		}

		std::vector<TOKEN> params;
		if (expr->params.size() > 0){
			Trace("Validating Func Call Params For", expr->name.c_str());
			for (int j = 0; j < expr->params.size(); j++) {
				TOKEN type = ValidateExpr(expr->params[j]);
				params.push_back(type);
			}
		}
		SetRowCol(expr);

		for (int i = 0; i < nodes.size(); i++){
			if (nodes[i]->type == FUNC){
				if (staticRequired && !nodes[i]->HasVisibility(STATIC)) continue;
				ASTFunc* func = (ASTFunc*) nodes[i];
				bool notIt = false;

				if (funcs > 1){ // if more than one function exists in the object, then we'll check parameters
					if (func->args.size() < params.size()) continue;
					for (int j = 0; j < func->args.size(); j++){
						if (j < params.size()){
							if (func->args[j]->baseType != VAR && func->args[j]->baseType != params[j]) {
								notIt = true;
							}
						}
					}
				}
				if (notIt) continue;

				expr->func = func;
				indent++;
				ValidateFunc(func, true, isConstructor);
				indent--;	
				if (isConstructor) {
					expr->func->assignmentType = OBJECT;
				}
				return expr->func->assignmentType;
			}
		}
		if (expr->func == NULL && isConstructor) throw Error::OBJECT_CONSTRUCTOR_INVALID;
		else if (expr->func == NULL) throw Error::INVALID_FUNCTION_CALL;
		return UNDEFINED;
	}

	void Semantics::ValidateFunc(ASTFunc* func, bool parse, bool isConstructor){
		if (func->scope->IsParsed() || !parse) {
			for (int i = 0; i < func->args.size(); i++){ // validate args
				ASTNode* arg = func->args[i];
				bool found = false;
				for (int j = 0; j < func->scope->Size(); j++){
					if (func->scope->Get(j) == arg){
						found = true;
						break;
					}
				}
				if (found) break;
				func->scope->InsertBefore(arg);
				// scan scope
				ScanScope(func->scope);
			}
			return;
		}
		Trace("Parsing Func", func->name.c_str());
		func->scope = Parse(func->scope);
		if (func->scope->outer == NULL) func->scope->outer = GetCurrentScope(); // this will happen ?
		if (func->scope->owner == NULL) func->scope->owner = func;
		for (int i = 0; i < func->args.size(); i++){
			ASTVar* var = (ASTVar*) func->args[i];
			var->isArg = true;
			var->order = i;
			func->scope->InsertBefore(func->args[i]);
		}
		ScanScope(func->scope);
		std::vector<ASTNode*> returns = func->scope->Lookup("return", false);
		if (returns.size() > 1) {
			SetRowCol(returns[1]);
			throw Error::MULTIPLE_RETURNS_NOT_ALLOWED;
		}
		else if (returns.size() == 1 && isConstructor) throw Error::NO_RETURN_STMTS_IN_CONSTRUCTOR;
		else if (returns.size() == 1 && !isConstructor) {
			ASTVar* r = (ASTVar*) returns[0];
			TOKEN castType = GetCastType(r);
			func->assignmentType = ((ASTVar*) returns[0])->assignmentType;
		}
	}

	TOKEN Semantics::GetCastType(ASTNode* node){
		if (node == NULL) return UNDEFINED;
		int type = (int) node->type;
		switch (type){
			case VAR:{
				ASTVar* var = (ASTVar*) node;
				return GetCastType(var->value);
			}
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				if (lit->cast != NULL){
					int a = 10;
					HERE();
				}
			}
			case BINARY: {
				return UNDEFINED;
			}
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateArray(ASTArray* expr){
		TOKEN type = ValidateExpr(expr->members[0]);
		for (int i = 1; i < expr->members.size(); i++){
			TOKEN tmpType = ValidateExpr(expr->members[i]);
			SetRowCol(expr->members[i]);
			if (type != tmpType) throw Error::INVALID_ARRAY_MEMBER;
		}
		return type;
	}

	void Semantics::ExpectNumber(ASTLiteral* lit){
		SetRowCol(lit);
		int type = (int) lit->litType;
		if (lit->var != NULL){
			type = (int) lit->var->assignmentType;
		}
		switch (type){
			case BOOLEAN: case INT: case DOUBLE: case FLOAT: return;
		}
		throw Error::INVALID_ACCESS_TO_ARRAY;
	}

	void Semantics::ValidateObject(ASTObject* obj){
		Trace("Validating Object", obj->name.c_str());
	}

	void Semantics::ValidateObjectInit(ASTVar* var){
		if (var->value->type != FUNC_CALL) throw Error::OBJECT_CONSTRUCTOR_INVALID;
		Trace("Validating Var", var->name.c_str());
		indent++;
		Trace("Object", "True");
		var->baseName = var->value->name;
		std::vector<ASTNode*> nodes = GetCurrentScope()->Lookup(var->baseName);
		ASTObject* base = NULL;
		if (nodes.size() > 1){
			for (int i = 0; i < nodes.size(); i++){ // get the base object
				ASTObject* obj = (ASTObject*) nodes[i];
				if (obj->type == OBJECT && !obj->extend){
					base = obj;
				}
			}
			base->scope = Parse(base->scope);
			ScanScope(base->scope);
			for (int i = 0; i < nodes.size(); i++){
				ASTObject* obj = (ASTObject*) nodes[i];
				if (obj->type == OBJECT && obj->extend){
					ValidateExtend(base, obj);
					Scope* s = obj->scope;
					obj->scope->outer->Destroy(obj);
					s->Destroy();
				}
			}
		}
		else if (nodes.size() == 0) throw Error::UNDEFINED_VARIABLE;
		else{
			ASTObject* obj = (ASTObject*) nodes[0];
			if (obj->type != OBJECT) throw Error::UNDEFINED_OBJECT;
			base = obj;
			base->scope = Parse(base->scope);
			ScanScope(base->scope);
		}

		// find the matching constructor
		Trace("Validating Object", var->value->name.c_str());
		OpenScope(base->scope);
		ValidateFuncCall((ASTFuncCall*) var->value, true);
		CloseScope();
		indent--;

		ASTObjectInstance* instance = ASTObjectInstance::New(isolate);
		instance->constructor = (ASTFuncCall*) var->value;
		instance->base = base;
		instance->litType = OBJECT;
		var->value = instance;
	}

	void Semantics::ValidateExtend(ASTObject* base, ASTObject* extend){
		Trace("Extending Object", base->name.c_str());
		indent++;
		extend->scope = Parse(extend->scope);
		ScanScope(extend->scope);
		base->scope->Merge(extend->scope);
		indent--;
	}

	TOKEN Semantics::ValidateObjectChain(ASTBinaryExpr* expr){
		SetRowCol(expr);
		Scope* s = GetCurrentScope();
		ValidateExpr(expr->left);
		ASTObject* obj = GetObject(expr->left);
		bool allowAccess = false;

		if (obj == NULL) {
			ASTLiteral* lit = (ASTLiteral*) expr->left;
			throw Error::UNDEFINED_OBJECT;
		}
		if (expr->left->type == LITERAL){
			ASTLiteral* lit = (ASTLiteral*) expr->left;
			lit->obj = obj;
			if (lit->value == "this") {
				lit->litType = OBJECT;
				allowAccess = true;
				isThis = true;
			}
		}
		if (!obj->scope->IsParsed()) {
			obj->scope = Parse(obj->scope);
			ScanScope(obj->scope);
		}

		inObject = true;
		indent++;
		OpenScope(obj->scope);
		TOKEN right = ValidateExpr(expr->right);
		CloseScope();

		if (expr->right->type == LITERAL){
			ASTLiteral* lit = (ASTLiteral*) expr->right;
			lit->allowAccess = allowAccess;
		}

		indent--;
		inObject = false;
		//if (isThis) CloseScope(); // here
		isThis = false;
		return right;
	}

	ASTObject* Semantics::GetObject(ASTNode* node){
		if (node == NULL) return NULL;
		SetRowCol(node);
		if (isThis) {
			ASTObject* o = kThis;
			kThis = NULL;
			return o;
		}
		int type = (int) node->type;
		switch (type){
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				if (lit->obj != NULL) return lit->obj;
				return GetObject(lit->var);
			}
			case VAR: {
				ASTVar* var = (ASTVar*) node;
				if (var->isObject) return GetObject(var->value);
				ASTObject* obj = GetObject(var->value);
				if (obj == NULL && var->baseName.length() > 0){ // object chain (apple.class.name)
					Scope* s = GetCurrentScope();
					std::vector<ASTNode*> results = s->Lookup(var->baseName);
					for (int i = 0; i < results.size(); i++){
						if (results[i]->type == OBJECT){
							obj = (ASTObject*) results[i];
							break;
						}
					}
					return obj;
				}
				return obj;
			}
			case OBJECT_INSTANCE: {
				ASTObjectInstance* instance = (ASTObjectInstance*) node;
				return instance->base;
			}
			case OBJECT: return (ASTObject*) node;
			case FUNC_CALL: {
				ASTFuncCall* call = (ASTFuncCall*) node;
				if (call->func == NULL) {
					SetRowCol(call);
					throw Error::UNDEFINED_FUNC;
				}
				if (!call->func->scope->IsParsed()) ValidateFunc(call->func);
				std::vector<ASTNode*> lookup = call->func->scope->Lookup("return", false);
				if (lookup.size() == 0) return NULL;
				OpenScope(call->func->scope);
				ASTObject* o = GetObject(lookup[0]);
				CloseScope();
				return o;
			}
			default: {
				printf("dd: %s - %s\n", Token::ToString(node->type).c_str(), node->name.c_str());
				return NULL;
			}
		}
		return NULL;
	}

	TOKEN Semantics::ValidateCast(ASTExpr* expr){
		Trace("Validating", "Cast");
		ValidateLiteral((ASTLiteral*) expr->cast);
		return expr->cast->litType;
	}

	// TODO: Compare the base type with the assignment type. Throw an error if they are not the same if hard typed.
	void Semantics::ValidateBaseAndAssignment(ASTVar* var){
		
	}

	TOKEN Semantics::ValidateWhile(ASTWhileExpr* expr){
		Trace("Validating", "While");
		ValidateBoolean((ASTBinaryExpr*) expr->condition);
		expr->scope = Parse(expr->scope);
		expr->scope->InsertBefore(expr->condition);
		ScanScope(expr->scope);
		expr->scope->Remove(expr->condition);
		return UNDEFINED;
	}

	/**
	 * All Internal Functions need to be registered in Internal::Bind()
	 */
	TOKEN Semantics::ValidateInternal(ASTFuncCall* call){
		TOKEN t = Internal::Bind(call);
		for (int i = 0; i < call->params.size(); i++) ValidateExpr(call->params[i]);
		Trace("  Type: ", "Internal");
		return t;
	}

	Scope* Semantics::ParseAndScan(Scope* scope){
		Scope* outer = scope->outer;
		Scope* s = Parse(scope);
		s->outer = outer;
		ScanScope(s);
		return s;
	}
} // namespace internal
}