// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "semantics.h"

namespace Condor {
namespace internal{

	Semantics::Semantics(Isolate* iso){
		isolate = iso;
		row = -1;
		col = -1;
		baseScope = NULL;
		trace = TRACE_SEMANTIC;
		indent = 0;
		inObject = false;
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
			isolate->FreeMemory(t, sizeof(Token));
		}
	}

	Scope* Semantics::Parse(Scope* scope){
		try {
			return Parser::Parse(isolate, scope, this);
		}
		catch (Error::CB_ERROR e){
			OpenScope(scope);
			throw e;
		}
	}

	Scope* Semantics::GetCurrentScope(){
		if (scopes.size() == 0) return NULL;
		return scopes[0];
	}

	void Semantics::SwapScopes(){
		CHECK(scopes.size() >= 2);
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
		CHECK(scope != NULL);
		Trace("Scanning Scope", (std::to_string(scope->Size()) + " items").c_str());
		indent++;
		if (scope->outer == NULL) scope->outer = GetCurrentScope();
		OpenScope(scope);
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			TRACK(node);
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
					CHECK(func->scope != NULL);
					func->scope->outer = scope;
					break;
				}
				case OBJECT: {
					ASTObject* obj = (ASTObject*) node;
					CHECK(obj->scope != NULL);
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
		TRACK(expr);
		CHECK(expr != NULL);
		SetRowCol(expr);
		if (expr->isDefault && expr->condition != NULL) throw Error::INVALID_DEFAULT_STMT;
		else if (expr->condition != NULL && expr->condition->type != LITERAL) throw Error::INVALID_CASE_STMT;
		ValidateExpr((ASTExpr*) expr->condition);
	}

	void Semantics::ValidateSwitch(ASTSwitch* expr){
		Trace("Validating", "Switch");
		TRACK(expr);
		CHECK(expr != NULL);
		SetRowCol(expr);
		ValidateExpr((ASTExpr*) expr->value);
		for (unsigned int i = 0; i < expr->cases.size(); i++){
			ValidateCase(expr->cases[i]);
		}
	}

	void Semantics::ValidateVar(ASTVar* var){
		SetRowCol(var);
		TRACK(var);
		CHECK(var != NULL);
		std::vector<ASTNode*> existingNodes = GetCurrentScope()->Lookup(var->name, false); // check if current var exists in the current scope
		if (existingNodes.size() > 1 && existingNodes[0] != var) throw Error::NAME_ALREADY_EXISTS; // don't show the first variable

		var->scopeId = GetCurrentScope()->scopeId;
		if (var->isObject && var->value->type == FUNC_CALL){
			ValidateObjectInit(var);
			return;
		}
		Trace("Validating Var", var->name.c_str());
		indent++;
		var->assignmentType = ValidateExpr((ASTExpr*) var->value); // here TODO: recursive

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
		else if (var->isArray){
			ASTArray* ary = (ASTArray*) var->value;
			CHECK(ary != NULL);
			if (ary->type != UNDEFINED){
				for (unsigned int i = 0; i < ary->members.size(); i++){
					ValidateExpr(ary->members[i]);
				}
			}
		}
		ValidateBaseAndAssignment(var);
		if (var->previouslyDeclared){
			ASTLiteral* member = (ASTLiteral*) var->member;
			CHECK(member != NULL);
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
		TRACK(expr);
		int type = (int) expr->type;
		switch (type){
			case FUNC_CALL: {
				ASTFuncCall* call = (ASTFuncCall*) expr;
				CHECK(call->func != NULL);
				std::vector<ASTNode*> nodes = call->func->scope->Lookup("return", false);
				if (nodes.size() == 0) return "";
				ASTVar* returnVar = (ASTVar*) nodes[0];
				return returnVar->baseName;
			}
			default: {
				printf("ddd: %s\n", Token::ToString(expr->type).c_str());
				throw Error::NOT_IMPLEMENTED;
			}
		}
		return "";
	}

	TOKEN Semantics::ValidateExpr(ASTExpr* expr){
		SetRowCol(expr);
		if (expr == NULL) return UNDEFINED;
		TRACK(expr);
		if (expr->cast != NULL) {
			ValidateCast(expr);
		}
		int type = (int) expr->type;
		switch (type){
			case UNDEFINED: return UNDEFINED;
			case BINARY: return ValidateBinary((ASTBinaryExpr*) expr);
			case LITERAL: return ValidateLiteral((ASTLiteral*) expr);
			case FUNC_CALL: return ValidateFuncCall((ASTFuncCall*) expr);
			case ARRAY: return ValidateArray((ASTArray*) expr);
			case BREAK: return UNDEFINED;
			case CONTINUE: return UNDEFINED;
			default: {
				throw Error::NOT_IMPLEMENTED;
			}
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateLiteral(ASTLiteral* lit){
		SetRowCol(lit);
		TRACK(lit);
		Trace("Literal Value", lit->value.c_str());
		int type = (int) lit->litType;
		TOKEN result = UNDEFINED;
		switch (type){
			case IDENT: result = ValidateIdent(lit); break;
			default:break;
		}
		if (lit->member != NULL){
			ValidateExpr(lit->member);
			if (lit->var != NULL) result = lit->var->baseType;
		}
		if (result == UNDEFINED) return lit->litType;
		return result;
	}

	void Semantics::ValidateFor(ASTForExpr* expr){
		CHECK(expr != NULL);
		TRACK(expr);
		SetRowCol(expr);
		Trace("Validating", "For");
		indent++;
		ValidateBoolean((ASTBinaryExpr*) expr->condition);
		expr->scope = Parse(expr->scope);
		expr->scope->InsertBefore(expr->tick); // i++
		expr->scope->InsertBefore(expr->condition); // i < 10;
		expr->scope->InsertBefore(expr->var); // var i = 0;
		ScanScope(expr->scope);
		expr->scope->Remove(expr->tick);
		expr->scope->Remove(expr->condition);
		indent--;
	}

	void Semantics::ValidateBoolean(ASTBinaryExpr* expr){
		CHECK(expr != NULL);
		SetRowCol(expr);
		if (!expr->isBoolean) throw Error::INVALID_BOOLEAN_COMPARISON;
	}

	void Semantics::ValidateIf(ASTIf* expr){
		if (expr == NULL) return;
		SetRowCol(expr);
		TRACK(expr);
		Trace("Validating", "If");
		if (!expr->isElse){
			ValidateExpr(expr->condition);
			ValidateBoolean((ASTBinaryExpr*) expr->condition);
		}
		indent++;
		expr->scope = Parse(expr->scope);
		ScanScope(expr->scope);
		indent--;
		for (unsigned int i = 0; i < expr->elseIfs.size(); i++) ValidateIf(expr->elseIfs[i]);
	}

	TOKEN Semantics::ValidateBinary(ASTBinaryExpr* expr){
		CHECK(expr != NULL);
		TRACK(expr);
		if (expr->op == PERIOD) return ValidateObjectChain(expr);;
		SetRowCol(expr);
		ValidateExpr(expr->left);
		isThis = false; // TODO: Verify this is ok and always true

		staticRequired = false;
		if (expr->left != NULL && expr->left->type == OBJECT) staticRequired = true;

		bool working = inObject; // used for object chains
		if (inObject){
			SwapScopes();
			inObject = false;
		}

		ValidateExpr(expr->right);

		if (working){
			SwapScopes();
			RemoveThis();
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateIdent(ASTLiteral* expr){
		CHECK(expr != NULL);
		TRACK(expr);
		SetRowCol(expr);
		expr->name = expr->value;
		Trace("Validating Ident", expr->name.c_str());
		Scope* s = GetCurrentScope();
		if (expr->name == "this"){
			if (s->outer == NULL) throw Error::INVALID_USAGE_OF_THIS;
			ASTObject* o = GetObjectFromScope(s->outer);
			if (o != NULL) {
				AddThis(o);
			}
			else throw Error::INVALID_USAGE_OF_THIS;
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
		for (unsigned int i = 0; i < nodes.size(); i++){
			if ((!isThis && !expr->allowAccess) && nodes[i]->HasVisibility(PRIVATE)) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
			if ((nodes[i]->type == VAR || nodes[i]->type == OBJECT)) {
				expr->var = (ASTVar*) nodes[i];
				Scope* currentScope = GetCurrentScope();
				expr->allowGC = currentScope->scopeId == expr->var->scopeId;
				break;
			}
		}

		if (expr->var == NULL) {
			throw Error::UNDEFINED_VARIABLE;
		}
		CHECK(expr->var != NULL);
		if (expr->var != NULL && expr->var->assignmentType == UNDEFINED) return expr->var->baseType;
		return expr->var->assignmentType;
	}

	TOKEN Semantics::ValidateFuncCall(ASTFuncCall* expr, bool isConstructor){
		CHECK(expr != NULL);
		TRACK(expr);
		if (!isConstructor) isConstructor = expr->isInit;
		Trace("Validating Func Call", expr->name.c_str());
		SetRowCol(expr);
		if (expr->isInternal) return ValidateInternal(expr);
		int funcs = 0;
		std::vector<ASTNode*> nodes = GetCurrentScope()->Lookup(expr->name);
		if (isConstructor){
			for (unsigned int i = 0; i < nodes.size(); i++){
				if (nodes[i]->type == OBJECT){
					ASTObject* obj = (ASTObject*) nodes[i];
					if (!obj->scope->IsParsed()) obj->scope = Parse(obj->scope);
					CHECK(obj->scope != NULL);
					nodes = obj->scope->Lookup(expr->name, false);
					break;
				}
			}
		}

		if (nodes.size() == 0) throw Error::UNDEFINED_FUNC;
		for (unsigned int i = 0; i < nodes.size(); i++){ // count functions, if funcs > 1, then worry about the parameters
			if (nodes[i]->type == FUNC) funcs++;
		}

		std::vector<TOKEN> params;
		if (expr->params.size() > 0){
			Trace("Validating Func Call Params For", expr->name.c_str());
			for (unsigned int j = 0; j < expr->params.size(); j++) {
				TOKEN type = ValidateExpr(expr->params[j]);
				params.push_back(type);
			}
		}
		SetRowCol(expr);

		for (unsigned int i = 0; i < nodes.size(); i++){
			if (nodes[i]->type == FUNC){
				if (staticRequired && !nodes[i]->HasVisibility(STATIC)) continue;
				ASTFunc* func = (ASTFunc*) nodes[i];
				CHECK(func != NULL);
				bool notIt = false;

				if (funcs > 1){ // if more than one function exists in the object, then we'll check parameters
					if (func->args.size() < params.size()) continue;
					for (unsigned int j = 0; j < func->args.size(); j++){
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
		CHECK(func != NULL);
		TRACK(func);
		func->progressDepth++;
		if (func->progressDepth > 1) return;
		if ((func->scope->IsParsed() || !parse) && !func->scope->IsShallow()) {
			for (unsigned int i = 0; i < func->args.size(); i++){ // validate args
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
				func->scope->UnsetShallow();
			}
			return;
		}
		Trace("Parsing Func", func->name.c_str());
		func->scope = Parse(func->scope);
		if (func->importScopeId > -1) {
			Scope* outer = isolate->GetContext()->GetFromRegistry(func->importScopeId);
			CHECK(outer != NULL);
			func->scope->outer = outer;
		}
		if (func->scope->outer == NULL) func->scope->outer = GetCurrentScope(); // this will happen ?
		if (func->scope->owner == NULL) func->scope->owner = func;
		for (unsigned int i = 0; i < func->args.size(); i++){
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
			GetCastType(r);
			func->assignmentType = ((ASTVar*) returns[0])->assignmentType;
		}
		func->progressDepth--;
	}

	TOKEN Semantics::GetCastType(ASTNode* node){
		if (node == NULL) return UNDEFINED;
		TRACK(node);
		int type = (int) node->type;
		switch (type){
			case VAR:{
				ASTVar* var = (ASTVar*) node;
				return GetCastType(var->value);
			}
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				if (lit->cast != NULL) return lit->cast->litType;
			}
			case BINARY: {
				return UNDEFINED;
			}
			default:break;
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateArray(ASTArray* expr){
		CHECK(expr != NULL);
		if (expr->members.size() == 0) return UNDEFINED;
		TOKEN type = ValidateExpr(expr->members[0]);
		for (unsigned int i = 1; i < expr->members.size(); i++){
			TOKEN tmpType = ValidateExpr(expr->members[i]);
			SetRowCol(expr->members[i]);
			if (type != tmpType) throw Error::INVALID_ARRAY_MEMBER;
		}
		return type;
	}

	void Semantics::ExpectNumber(ASTLiteral* lit){
		CHECK(lit != NULL);
		TRACK(lit);
		SetRowCol(lit);
		int type = (int) lit->litType;
		if (lit->var != NULL){
			type = (int) lit->var->assignmentType;
		}
		switch (type){
			case BOOLEAN: case INT: case DOUBLE: case FLOAT: return;
			default:break;
		}
		throw Error::INVALID_ACCESS_TO_ARRAY;
	}

	void Semantics::ValidateObjectInit(ASTVar* var){
		CHECK(var != NULL);
		if (var->value->type != FUNC_CALL) throw Error::OBJECT_CONSTRUCTOR_INVALID;
		Trace("Validating Var", var->name.c_str());
		indent++;
		Trace("Object", "True");
		var->baseName = var->value->name;
		std::vector<ASTNode*> nodes = GetCurrentScope()->Lookup(var->baseName);
		ASTObject* base = NULL;
		if (nodes.size() > 1){
			for (unsigned int i = 0; i < nodes.size(); i++){ // get the base object
				ASTObject* obj = (ASTObject*) nodes[i];
				if (obj->type == OBJECT && !obj->extend){
					base = obj;
				}
			}
			CHECK(base != NULL);
			base->scope = Parse(base->scope);
			ScanScope(base->scope);
			for (unsigned int i = 0; i < nodes.size(); i++){
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
		CHECK(instance->base != NULL);
		instance->litType = OBJECT;
		var->value = instance;
	}

	void Semantics::ValidateExtend(ASTObject* base, ASTObject* extend){
		CHECK(base != NULL);
		CHECK(extend != NULL);
		TRACK(base);
		TRACK(extend);
		Trace("Extending Object", base->name.c_str());
		indent++;
		extend->scope = Parse(extend->scope);
		ScanScope(extend->scope);
		base->scope->Merge(extend->scope);
		indent--;
	}

	TOKEN Semantics::ValidateObjectChain(ASTBinaryExpr* expr){
		CHECK(expr != NULL);
		TRACK(expr);
		SetRowCol(expr);
		// Scope* s = GetCurrentScope();
		ValidateExpr(expr->left);
		ASTObject* obj = GetObject(expr->left);
		bool allowAccess = false;

		if (obj == NULL) {
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
		isThis = false;
		return right;
	}

	ASTObject* Semantics::GetObjectFromScope(Scope* scope){
		if (scope == NULL) return NULL;
		if (scope->owner == NULL) return NULL;
		if (scope->owner->type == OBJECT) return (ASTObject*) scope->owner;
		return GetObjectFromScope(scope->outer);
	}

	ASTObject* Semantics::GetObject(ASTNode* node){
		if (node == NULL) return NULL;
		TRACK(node);
		SetRowCol(node);
		if (isThis) {
			if (kThis.size() == 0 && node->type == LITERAL){
				ASTLiteral* lit = (ASTLiteral*) node;
				CHECK(lit != NULL && lit->obj != NULL);
				AddThis(lit->obj);
			}
			CHECK(kThis.size() > 0);
			ASTObject* o = kThis[0];
			RemoveThis();
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
					for (unsigned int i = 0; i < results.size(); i++){
						if (results[i]->type == OBJECT){
							obj = (ASTObject*) results[i];
							break;
						}
					}
					if (obj == NULL){ // TODO: Is this true always?
						obj = GetObjectFromScope(GetCurrentScope());
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
				CHECK(call->func != NULL);
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
		CHECK(expr != NULL);
		TRACK(expr);
		Trace("Validating", "Cast");
		ValidateLiteral(expr->cast);
		return expr->cast->litType;
	}

	// TODO: Compare the base type with the assignment type. Throw an error if they are not the same if hard typed.
	void Semantics::ValidateBaseAndAssignment(ASTVar* var){
		CHECK(var != NULL);
		TRACK(var);
		Trace("Validating", "Base type and the assignment type");
		int baseType = (int) var->baseType;
		TOKEN assign = var->assignmentType;
		if (assign == UNDEFINED) return;
		switch (baseType){
			case BOOLEAN: if (assign != FALSE_LITERAL && 
												assign != TRUE_LITERAL && 
												assign != BOOLEAN) throw Error::INVALID_BOOLEAN_ASSIGNMENT; break;
			case CHAR: if (assign != INT && 
										 assign != CHAR) throw Error::INVALID_CHAR_ASSIGNMENT; break;
			case INT: if (assign != INT && 
										assign != CHAR && 
										assign != BOOLEAN && 
										assign != TRUE_LITERAL &&
										assign != FALSE_LITERAL) throw Error::INVALID_INT_ASSIGNMENT; break;
			case FLOAT: if (assign != FLOAT &&
											assign != INT &&
											assign != BOOLEAN) throw Error::INVALID_FLOAT_ASSIGNMENT; break;
			case DOUBLE: if (assign != DOUBLE && 
											 assign != FLOAT &&
											 assign != INT &&
											 assign != BOOLEAN) throw Error::INVALID_DOUBLE_ASSIGNMENT; break;
			case ARRAY: if (assign != ARRAY) throw Error::INVALID_ARRAY_ASSIGNMENT; break;
			default:break;
		}
		if (var->baseType == DOUBLE && var->assignmentType == FLOAT){
			var->assignmentType = DOUBLE;
			SetToType(DOUBLE, var);
		}
	}

	// TODO: Incomplete crawling of all types.
	void Semantics::SetToType(TOKEN type, ASTNode* node){
		CHECK(node != NULL);
		TRACK(node);
		int whichType = (int) node->type;
		switch (whichType){
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				lit->litType = type;
				break;
			}
			case VAR: {
				ASTVar* var = (ASTVar*) node;
				SetToType(type, var->value);
				break;
			}
			case BINARY: {
				ASTBinaryExpr* binary = (ASTBinaryExpr*) node;
				SetToType(type, binary->left);
				SetToType(type, binary->right);
				break;
			}
			default:break;
		}
	}

	TOKEN Semantics::ValidateWhile(ASTWhileExpr* expr){
		CHECK(expr != NULL);
		TRACK(expr);
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
		CHECK(call != NULL);
		TRACK(call);
		TOKEN t = Internal::Bind(call);
		for (unsigned int i = 0; i < call->params.size(); i++) ValidateExpr(call->params[i]);
		Trace("  Type: ", "Internal");
		return t;
	}

	Scope* Semantics::ParseAndScan(Scope* scope){
		CHECK(scope != NULL);
		Scope* outer = scope->outer;
		Scope* s = Parse(scope);
		s->outer = outer;
		ScanScope(s);
		return s;
	}
} // namespace internal
}