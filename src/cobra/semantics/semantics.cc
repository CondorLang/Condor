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
		baseScope = scope;
		ScanScope(baseScope);
	}

	void Semantics::ScanScope(Scope* scope){
		Trace("Scanning Scope", (std::to_string(scope->Size()) + " items").c_str());
		scope->outer = GetCurrentScope();
		OpenScope(scope);
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			SetRowCol(node);
			int type = (node->type);
			switch (type){
				case VAR: ValidateVar((ASTVar*) node); break;
				case FOR: ValidateFor((ASTForExpr*) node); break;
				case WHILE: ValidateWhile((ASTWhileExpr*) node); break;
				case IF: ValidateIf((ASTIf*) node); break;
				case FUNC: /**ValidateFunc((ASTFunc*) node);**/ break; // Part of JIT compiling
				case OBJECT: /**ValidateObject((ASTObject*) node);**/ break; // Part of JIT compiling
				default: ValidateExpr((ASTExpr*) node); break;
			}
		}
		CloseScope();
	}

	void Semantics::ValidateVar(ASTVar* var){
		SetRowCol(var);
		if (var->isObject && var->value->type == FUNC_CALL){
			ValidateObjectInit(var);
			return;
		}
		Trace("Validating Var", var->name.c_str());
		indent++;
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
		indent--;
	}

	TOKEN Semantics::ValidateExpr(ASTExpr* expr){
		SetRowCol(expr);
		if (expr == NULL) return UNDEFINED;
		if (expr->cast != NULL) return ValidateCast(expr);
		int type = (int) expr->type;
		switch (type){
			case UNDEFINED: return UNDEFINED;
			case BINARY: return ValidateBinary((ASTBinaryExpr*) expr);
			case LITERAL: return ValidateLiteral((ASTLiteral*) expr);
			case FUNC_CALL: return ValidateFuncCall((ASTFuncCall*) expr);
			case ARRAY: return ValidateArray((ASTArray*) expr);
			default: {
				printf("d: %s\n", Token::ToString(expr->type).c_str());
				throw Error::NOT_IMPLEMENTED;
			}
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateLiteral(ASTLiteral* lit){
		SetRowCol(lit);
		Trace("Literal Value", lit->value.c_str());
		int type = (int) lit->litType;
		switch (type){
			case IDENT: return ValidateIdent(lit);
		}
		return lit->litType;
	}

	void Semantics::ValidateFor(ASTForExpr* expr){
		SetRowCol(expr);
		Trace("Validating", "For");
		indent++;
		ValidateBoolean((ASTBinaryExpr*) expr->condition);
		expr->scope = Parse(expr->scope);
		expr->scope->InsertBefore(expr->var);
		ScanScope(expr->scope);
		indent--;
	}

	void Semantics::ValidateBoolean(ASTBinaryExpr* expr){
		SetRowCol(expr);
		if (!expr->isBoolean) throw Error::INVALID_BOOLEAN_COMPARISON;
	}

	void Semantics::ValidateIf(ASTIf* expr){
		SetRowCol(expr);
		Trace("Validating", "If");
		ValidateBoolean((ASTBinaryExpr*) expr->condition);
		indent++;
		expr->scope = Parse(expr->scope);
		ScanScope(expr->scope);
		indent--;
	}

	TOKEN Semantics::ValidateBinary(ASTBinaryExpr* expr){
		if (expr->op == PERIOD) return ValidateObjectChain(expr);;
		SetRowCol(expr);
		TOKEN left = ValidateExpr(expr->left);

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
		}

		try{
			return Binary::Compare(left, right, expr->op);
		}
		catch (Error::ERROR e) {
			SetRowCol(expr->left);
			throw e;
		}
	}

	TOKEN Semantics::ValidateIdent(ASTLiteral* expr){
		Trace("Validating", "Ident");
		SetRowCol(expr);
		expr->name = expr->value;
		Scope* s = GetCurrentScope();
		if (expr->name == "this"){
			if (s->outer == NULL) throw Error::INVALID_USAGE_OF_THIS;
			kThis = (ASTObject*) s->outer->owner;
			OpenScope(s->outer);
			if (kThis == NULL) throw Error::INVALID_USAGE_OF_THIS;
			isThis = true;
			return UNDEFINED;
		}	
		std::vector<ASTNode*> nodes = s->Lookup(expr->name, isThis ? false : true);
		if (nodes.size() == 1 && nodes[0] == expr) throw Error::UNDEFINED_VARIABLE;

		for (int i = 0; i < nodes.size(); i++){
			if (!isThis && nodes[i]->HasVisibility(PRIVATE)) throw Error::UNABLE_TO_ACCESS_PRIVATE_MEMBER;
		}

		if (nodes.size() == 0) throw Error::UNDEFINED_VARIABLE;
		expr->var = (ASTVar*) nodes[0];
		if (expr->var->assignmentType == UNDEFINED) return expr->var->baseType;
		return expr->var->assignmentType;
	}

	// TODO: add variable types
	TOKEN Semantics::ValidateFuncCall(ASTFuncCall* expr, bool isConstructor){
		Trace("Validating Func Call", expr->name.c_str());
		SetRowCol(expr);
		if (expr->isInternal) return ValidateInternal(expr);
		std::vector<ASTNode*> nodes = GetCurrentScope()->Lookup(expr->name);
		if (nodes.size() == 0 && !isConstructor) throw Error::UNDEFINED_FUNC;
		for (int i = 0; i < nodes.size(); i++){
			if (nodes[i]->type == FUNC){
				if (staticRequired && !nodes[i]->HasVisibility(STATIC)) continue;
				ASTFunc* func = (ASTFunc*) nodes[i];
				ValidateFunc(func, true, isConstructor);
				expr->func = func;
				for (int i = 0; i < expr->params.size(); i++) ValidateExpr(expr->params[i]);
				return expr->func->assignmentType;
			}
		}
		return UNDEFINED;
	}

	// TODO: Fix RemoveAllAfter in scope. 
	void Semantics::ValidateFunc(ASTFunc* func, bool parse, bool isConstructor){
		if (func->scope->IsParsed() || !parse) return;
		Trace("Parsing Func", func->name.c_str());
		func->scope = Parse(func->scope);
		for (int i = 0; i < func->args.size(); i++){
			ASTVar* var = (ASTVar*) func->args[i];
			var->isArg = true;
			var->order = i;
			func->scope->InsertBefore(func->args[i]);
		}
		ScanScope(func->scope);
		std::vector<ASTNode*> returns = func->scope->Lookup("return", false);
		// if (returns.size() == 1) {
		// 	func->scope->RemoveAllAfter(returns[0]);
		// }
		if (returns.size() > 1) {
			SetRowCol(returns[1]);
			throw Error::MULTIPLE_RETURNS_NOT_ALLOWED;
		}
		else if (returns.size() == 1 && isConstructor) throw Error::NO_RETURN_STMTS_IN_CONSTRUCTOR;
		else if (returns.size() == 1 && !isConstructor) func->assignmentType = ((ASTVar*) returns[0])->assignmentType;
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
		ValidateExpr(expr->left);
		ASTObject* obj = GetObject(expr->left);
		if (obj == NULL) throw Error::UNDEFINED_OBJECT;
		if (!obj->scope->IsParsed()) {
			obj->scope = Parse(obj->scope);
			ScanScope(obj->scope);
		}

		inObject = true;
		indent++;
		OpenScope(obj->scope);
		TOKEN right = ValidateExpr(expr->right);
		CloseScope();
		indent--;
		inObject = false;
		isThis = false;
		return right;
	}

	ASTObject* Semantics::GetObject(ASTNode* node){
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
				return GetObject(lit->var);
			}
			case VAR: {
				ASTVar* var = (ASTVar*) node;
				if (var->isObject) return GetObject(var->value);
			}
			case OBJECT_INSTANCE: {
				ASTObjectInstance* instance = (ASTObjectInstance*) node;
				return instance->base;
			}
			case OBJECT: return (ASTObject*) node;
			default: {
				printf("dd: %s - %s\n", Token::ToString(node->type).c_str(), node->name.c_str());
				return NULL;
			}
		}
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
		ScanScope(expr->scope);
		return UNDEFINED;
	}

	/**
	 * All Internal Functions need to be registered here for now
	 */
	TOKEN Semantics::ValidateInternal(ASTFuncCall* call){
		if (call->name == "printf") call->callback = Internal::PrintF;
		else if (call->name == "readln") call->callback = Internal::ReadLine;
		else throw Error::UNDEFINED_FUNC;
		for (int i = 0; i < call->params.size(); i++) ValidateExpr(call->params[i]);
		Trace("  Type: ", "Internal");
		if (call->name == "readln") {
			int a = 10;
		}
		return UNDEFINED;
	}
} // namespace internal
}