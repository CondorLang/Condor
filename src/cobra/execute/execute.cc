#include "execute.h"

namespace Cobra {
namespace internal{

	Execute* Execute::New(Isolate* isolate, Scope* scope){
		void* pt = isolate->GetMemory(sizeof(Execute));
		Execute* n = new(pt) Execute(scope);
		n->isolate = isolate;
		return n;
	}

	Execute::Execute(Scope* scope){
		OpenScope(scope);
		trace = TRACE_EVALUATION;
		steps = EVALUATION_STEPS;
		row = -1;
		col = -1;
		canBreak = false;
		semantic = NULL;
		postixPeriod = false;
		numOfSteps = 1;
		rpnStack = RPN_STACK;
		PrintStep("\n\nStarting");
	}

	void Execute::Trace(std::string first, std::string msg2){
		if (trace) {
			std::string tabs = "";
			for (int i = 0; i < scopes.size(); i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first.c_str(), msg2.c_str());
		}
	}

	void Execute::PrintStep(std::string msg){
		if (steps) printf("Step [%d]: %s\n", numOfSteps++, msg.c_str());
	}

	void Execute::SetRowCol(ASTNode* node){
		if (node == NULL) return;
		row = node->row;
		col = node->col;
	}

	void Execute::OpenScope(Scope* sc){
		PrintStep("Opening Scope");
		if (sc->outer == NULL && scopes.size() > 0) sc->outer = scopes[scopes.size() - 1];
		scopes.insert(scopes.begin(), sc);
	}

	// TODO: Erase values in all non return locals
	void Execute::Evaluate(){
		PrintStep("Evaluating Scope");
		Scope* scope = GetCurrentScope();
		if (scope == NULL || scope->Size() == 0) return; // nothing to evaluate
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			int type = (int) node->type;
			switch (type){
				case FUNC_CALL: EvaluateFuncCall((ASTFuncCall*) node); break;
				case BINARY: EvaluateBinary((ASTBinaryExpr*) node); break;
				case VAR: EvaluateVar((ASTVar*) node); break;
				case FOR: EvaluateFor((ASTForExpr*) node); break;
				case WHILE: EvaluateWhile((ASTWhileExpr*) node); break;
				case IF: EvaluateIf((ASTIf*) node); break;
				case LITERAL: node->local = EvaluateValue(node); break;
				case SWITCH: EvaluateSwitch((ASTSwitch*) node); break;
				case BREAK: {
					if (canBreak) throw Error::INTERNAL_BREAK;
					throw Error::INVALID_USE_OF_BREAK;
				}
			}
		}
		CloseScope();
	}

	ASTLiteral* Execute::EvaluateFuncCall(ASTFuncCall* call){
		SetRowCol(call);
		Trace("Evaluating Func Call", call->name);
		PrintStep("Function Call - " + call->name);
		ASTFunc* func = call->func;
		if (func == NULL && call->isInternal){ // internal
			ASTLiteral* lit = NULL;
			if (call->params.size() > 0) {
				lit = EvaluateValue(call->params[0]);
				SetLitType(lit);
				if (lit != NULL) {
					lit = lit->Clone(isolate);
					isolate->RunGC(call->params[0], true);
				}
			}
			return (ASTLiteral*) Internal::CallInternal(isolate, call->callback, lit);
		}
		else{
			if (func == NULL) throw Error::UNDEFINED_FUNC;
			for (int i = 0; i < func->args.size(); i++){
				PrintStep("Evaluating Parameter (" + func->args[i]->name + ")");
				if (call->params.size() > i) {
					ASTLiteral* lit = EvaluateValue(call->params[i]);
					SetLitType(lit);
					if (lit == NULL) func->args[i]->local = ASTUndefined::New(isolate);
					else func->args[i]->local = lit->Clone(isolate);
				}
				else{
					func->args[i]->local = ASTUndefined::New(isolate);
				}
			}
			PrintStep("Evaluating Parameters...Done");
			//if (func->scope == GetCurrentScope()) throw Error::INTERNAL_SCOPE_ERROR;
			OpenScope(func->scope);
			Evaluate();
		}
		std::vector<ASTNode*> returns = func->scope->Lookup("return", false);
		if (returns.size() > 0) {
			ASTLiteral* lit = (ASTLiteral*) returns[0]->local;
			ASTLiteral* result = lit->Clone(isolate);
			PrintStep("Cloning (" + call->name + ") return value");
			isolate->RunGC(call, true);
			return result;
		}
		// TODO: Do we need to implement isolate->RunGC(call, true); ?
		return NULL;
	}

	ASTLiteral* Execute::EvaluateBinary(ASTBinaryExpr* binary){
		PrintStep("Binary Expression");
		SetRowCol(binary);
		if (binary->op == PERIOD && binary->right->type == FUNC_CALL && ((ASTFuncCall*) binary->right)->func->HasVisibility(STATIC)){
			PrintStep("Static Function Call");
			return (ASTLiteral*) EvaluateFuncCall((ASTFuncCall*) binary->right);
		}
		else{
			ASTToken* tok = ASTToken::New(isolate, binary->op);
			if (tok->value->IsAssign()){
				PrintStep("Binary Assignment");
				tok->Free(isolate);
				Assign(binary);
				return NULL;
			}
			else{
				PrintStep("Calculation");
				NewStack();
				FillPostix(binary);
				ASTLiteral* lit = Calculate();
				FormatLit(lit);
				CloseStack();
				return lit;
			}
		}
	}

	void Execute::SetCast(ASTExpr* expr, ASTLiteral* value){
		if (value == NULL) return;
		if (expr->cast != NULL){
			PrintStep("Casting value");
			ASTLiteral* lit = expr->cast;
			value->litType = lit->litType;
			FormatLit(value, true);
		}
	}

	void Execute::FormatLit(ASTLiteral* lit, bool forceType){
		if (lit == NULL) return;
		PrintStep("Formating Literal");
		SetCalc(lit);
		int type = (int) lit->litType;
		switch (type){
			case BOOLEAN: lit->value = (lit->calc == 0 ? "false" : "true"); break;
			case DOUBLE: lit->value = std::to_string(lit->calc); break;
			case FLOAT: lit->value = std::to_string((float) lit->calc); break;
			case INT: lit->value = std::to_string((int) lit->calc); break;
		}
		if (lit->calc - (int)lit->calc > 0.0 && !forceType) lit->value = std::to_string(lit->calc);
		TruncZeros(lit);
	}

	void Execute::TruncZeros(ASTLiteral* lit){
		PrintStep("Truncating Zeros");
		if (lit->value.empty()) return;
		std::string::size_type loc = lit->value.find(".", 0);
		if (loc == std::string::npos) return;
		for (int i = lit->value.size() - 1; i >= loc; i--){
			if (lit->value[i] == '0') lit->value.erase(i);
			else break;
		}
	}

	// TODO: Set row and col for tracking
	// TODO: String, Char, and other data types need to be implemented
	void Execute::FillPostix(ASTBinaryExpr* binary){
		PrintStep("Filling Postix (Reverse Polish Notation)");
		RPNStack* stack = GetCurrentStack();
		ASTLiteral* left = NULL;
		if (postixPeriod){
			left = (ASTLiteral*) binary->left;
		}
		else{
			left = EvaluateValue(binary->left);
		}
		ASTToken* tok = ASTToken::New(isolate, binary->op);
		postixPeriod = binary->op == PERIOD;

		if (left != NULL) stack->stack.push_back(left);
		if (binary->isInParen) stack->opStack.push_back(ASTToken::New(isolate, LPAREN));
		if (stack->opStack.size() == 0) stack->opStack.push_back(tok);
		else if (tok->value->value == LPAREN) stack->opStack.push_back(tok);
		else if (tok->value->value == RPAREN){
			for (int i = stack->opStack.size() - 1; i >= 0; i--){
				ASTToken* t = (ASTToken*) stack->opStack[i];
				if (t->value->value == LPAREN) break;
				else{
					stack->stack.push_back(t);
					stack->opStack.pop_back();
				}
				stack->opStack.pop_back();
			}
		}
		else if (((ASTToken*)(stack->opStack[stack->opStack.size() - 1]))->Precedence() < tok->Precedence()) stack->opStack.push_back(tok);
		else if (((ASTToken*)(stack->opStack[stack->opStack.size() - 1]))->Precedence() >= tok->Precedence()){
			for (int i = stack->opStack.size() - 1; i >= 0; i--){
				ASTToken* t = (ASTToken*) stack->opStack[i];
				if (t->Precedence() < tok->Precedence()) break;
				else{
					stack->stack.push_back(t);
					stack->opStack.pop_back();
				}
			}
			stack->opStack.push_back(tok);
		}
		if (binary->right != NULL && binary->right->type == BINARY) FillPostix((ASTBinaryExpr*) binary->right);
		else {
			postixPeriod = false;
			if (binary->op == PERIOD) stack->stack.push_back(binary->right);
			else stack->stack.push_back(EvaluateValue(binary->right));
		}
		if (binary->isInParen) {
			for (int i = stack->opStack.size() - 1; i >= 0; i--){
				ASTToken* t = (ASTToken*) stack->opStack[i];
				if (t->value->value == LPAREN) break;
				else{
					stack->stack.push_back(t);
					stack->opStack.pop_back();
				}
			}
			stack->opStack.pop_back();
		}
	}

	ASTLiteral* Execute::Calculate(){
		RPNStack* stack = GetCurrentStack();
		PrintStep("Evaluating Postix Values");
		for (int i = stack->opStack.size() - 1; i >= 0; i--){
			stack->stack.push_back(stack->opStack[i]);
			stack->opStack.pop_back();
		}

		std::vector<int> periods;
		for (int i = 0; i < stack->stack.size(); i++){
			if (stack->stack[i]->type == TOK){
				ASTToken* tok = (ASTToken*) stack->stack[i];
				if (tok->value->value == PERIOD) periods.push_back(i); // push index
			}
		}

		// --rpn-stack flag
		if (rpnStack){
			printf("\nStack Size: %lu\n", stack->stack.size());
			for (int i = 0; i < stack->stack.size(); i++){
				ASTNode* n = stack->stack[i];
				if (n->type == TOK){
					ASTToken* tok = (ASTToken*) n;
					printf("[%d]: %s\n", i, Token::ToString(tok->value->value).c_str());
				}
				else if (n->type == LITERAL){
					ASTLiteral* lit = (ASTLiteral*) n;
					printf("[%d]: %s\n", i, lit->value.c_str());
				}
				else if (n->type == OBJECT_INSTANCE){
					ASTObjectInstance* obj = (ASTObjectInstance*) n;
					printf("[%d]: obj(%s)\n", i, obj->base->name.c_str());
				}
				else if (n->type == FUNC_CALL){
					ASTFuncCall* call = (ASTFuncCall*) n;
					printf("[%d]: call(%s)\n", i, call->name.c_str());
				}
				else{
					printf("dd: %s\n", Token::ToString(n->type).c_str());
				}
			}
		}

		if (rpnStack) printf("\n--- Evaluating ---\n");

		// opStack is now the stack
		for (int i = 0; i < stack->stack.size(); i++){
			ASTNode* n = stack->stack[i];

			if (n->type == TOK){
				ASTToken* tok = (ASTToken*) n;
				if (rpnStack) printf("[%d]: %s\n", i, tok->value->String().c_str());
				ASTLiteral* val = Calc(tok);
				if (val != NULL) {
					PrintStep("Evaluated Value (" + val->value + ")");
					stack->opStack.push_back(val);
					if (rpnStack && val->value.length() > 0) printf("[%d] - [%d, %d, %d] = %s\n", i, i - 2, i - 1, i, val->value.c_str());
					else if (rpnStack) printf("[%d] - [%d, %d, %d] = [Object]\n",i,  i - 2, i - 1, i);
				}
				else throw Error::INVALID_OBJECT;
			}
			else if (n->type == LITERAL){
				ASTLiteral* lit = NULL;

				bool next = false;
				for (int j = 0; j < periods.size(); j++){
					if (periods[j] == i + 1){
						next = true;
						break;
					}
				}
				if (next) { // object oriented
					lit = (ASTLiteral*) n;
					if (rpnStack) printf("[%d]: %s (l)\n", i, lit->value.c_str());
					lit->litType = LITERAL;
					stack->opStack.push_back(lit);
				}
				else{
					if (rpnStack) printf("[%d]: %s (g)\n", i, ((ASTLiteral*)n)->value.c_str());
					lit = EvaluateValue(n);
					SetCalc(lit);
					stack->opStack.push_back(lit);
				}
			}
			else if (n->type == OBJECT_INSTANCE){
				ASTObjectInstance* lit = (ASTObjectInstance*) n;
				if (rpnStack) printf("[%d]: obj(%s)\n", i, lit->base->name.c_str());
				stack->opStack.push_back(lit);
			}
			else if (n->type == FUNC_CALL){
				ASTFuncCall* call = (ASTFuncCall*) n;
				if (rpnStack) printf("[%d]: call(%s)\n", i, call->name.c_str());
				stack->opStack.push_back(call);
			}
			else{
				printf("RPN Stack: %s\n", Token::ToString(n->type).c_str());
			}
		}
		if (stack->opStack.size() == 0) throw Error::INVALID_OPERATOR;
		return (ASTLiteral*) stack->opStack[0];
	}

	ASTLiteral* Execute::StackCall(ASTFuncCall* call, ASTToken* tok, ASTLiteral* first){
		if (tok->value->value == PERIOD){ // object oriented function call
			PrintStep("Calling object function (" + call->name + ")");
			AddObject((ASTObjectInstance*) first);
			ASTLiteral* result = EvaluateFuncCall(call);
			RemoveObject();
			return result;	
		}
		SetRowCol(call);
		throw Error::INVALID_FUNC_CALL;
	}

	ASTLiteral* Execute::Calc(ASTToken* tok){
		SetRowCol(tok);
		RPNStack* stack = GetCurrentStack();
		int type = (int) tok->value->value;
		if (stack->opStack.size() < 2) throw Error::INVALID_OPERATOR;
		ASTLiteral* first = (ASTLiteral*) stack->opStack[stack->opStack.size() - 1];
		stack->opStack.pop_back();
		if (tok->value->IsAssign()) return first;
		ASTLiteral* second = (ASTLiteral*) stack->opStack[stack->opStack.size() - 1];
		stack->opStack.pop_back();
		if (first->type == FUNC_CALL) return StackCall((ASTFuncCall*) first, tok, second);
		if (second->type == OBJECT_INSTANCE){
			ASTObjectInstance* inst = (ASTObjectInstance*) second;
			if ((inst->litType == OBJECT || inst->type == LITERAL) && inst->type != OBJECT_INSTANCE) inst = GetCurrentObject();
			ASTVar* var = inst->GetProp(isolate, first->value);
			ASTLiteral* val = EvaluateValue(var);
			return val;
		}

		ASTLiteral* result = ASTLiteral::New(isolate);
		SetLitType(first);
		SetLitType(second);

		result->litType = Binary::Compare(first->litType, second->litType, tok->value->value);

		PrintStep("Calculating ('" + second->value + "' " + tok->value->String() + " '" + first->value + "')");

		int litType = (int) result->litType;
		if (litType == BOOLEAN){
			litType = Binary::Compare(first->litType, second->litType, ADD);
		}
		// here
		//printf("d: %s - %s - %s - %s\n", Token::ToString((TOKEN) litType).c_str(), second->value.c_str(), first->value.c_str(), tok->value->String().c_str());
		switch (litType){
			case INT: case DOUBLE: 
			case FLOAT: case BOOLEAN: {
				switch (type){
					case ADD:	result->calc = second->calc + first->calc; break;
					case DIV:	result->calc = second->calc / first->calc; break;
					case MUL:	result->calc = second->calc * first->calc; break;
					case SUB:	result->calc = second->calc - first->calc; break;
					case MOD:	result->calc = (int) second->calc % (int) first->calc; break;
					case LOR:	result->calc = second->calc == 0 || first->calc == 0; break;
					case LAND:result->calc = second->calc != 0 && first->calc != 0; break;
					case EQL:	result->calc = second->calc == first->calc; break;
					case LSS:	result->calc = second->calc < first->calc; break;
					case GTR: result->calc = second->calc > first->calc; break;
					case LEQ: result->calc = second->calc <= first->calc; break;
					case GEQ: result->calc = second->calc >= first->calc; break;
					case NEQ: result->calc = second->calc != first->calc; break;
					default: {
						printf("debug int: %s\n", Token::ToString(tok->value->value).c_str());
					}
				}
				FormatLit(result);
				break;
			}
			case STRING: case CHAR: {
				if (second->litType == kNULL) second->value = "";
				switch (type){
					case ADD:	result->value = second->value + first->value; break;
					case LOR:	result->value = second->value.length() > 0 || first->value.length() > 0; break;
					case LAND:	result->value = second->value.length() != 0 && first->value.length() != 0; break;
					case EQL:	result->value = second->value == first->value; break;
					case NEQ: result->value = second->value != first->value; break;
					default: {
						printf("debug string: %s\n", Token::ToString(tok->value->value).c_str());
					}
				}
				FormatLit(result);
				break;
			}
		}
		return result;
	}

	void Execute::SetLitType(ASTLiteral* lit){
		if (lit != NULL && lit->litType == UNDEFINED && lit->value.length() > 0) lit->litType = STRING;
	}

	// TODO: Implement bitwise
	void Execute::Assign(ASTBinaryExpr* binary){
		PrintStep("Assignment value");
		SetRowCol(binary);	
		int type = (int) binary->op;
		switch (type){
			case ASSIGN: {
				ASTVar* var = GetVar(binary->left);
				ASTLiteral* lit = EvaluateValue(binary->right);
				if (var == NULL) {
					SetRowCol(binary->left);
					throw Error::INVALID_ASSIGNMENT_TO_TYPE;
				}
				if (var->local != NULL) isolate->RunGC(var, true);
				var->local = lit;
				break;
			}
			case ADD_ASSIGN: case SUB_ASSIGN: 
			case DIV_ASSIGN: case MUL_ASSIGN: {
				ASTVar* var = GetVar(binary->left);
				ASTLiteral* l = (ASTLiteral*) binary->right;
				ASTLiteral* lit = EvaluateValue(binary->right);
				if (var == NULL || var->local == NULL) {
					SetRowCol(binary->left);
					throw Error::INVALID_ASSIGNMENT_TO_TYPE;
				}
				ASTLiteral* local = (ASTLiteral*) var->local;
				switch (type){
					case SUB_ASSIGN: local->calc -= lit->calc; break;
					case ADD_ASSIGN: local->calc += lit->calc; break;
					case MUL_ASSIGN: local->calc *= lit->calc; break;
					case DIV_ASSIGN: local->calc /= lit->calc; break;
				}
				FormatLit(local);
				break;
			}
		}
	}

	ASTVar* Execute::GetVar(ASTNode* node){
		PrintStep("Getting Variable (" + node->name + ")");
		SetRowCol(node);
		if (node == NULL) return NULL;
		int type = (int) node->type;
		switch (type){
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				if (lit->var != NULL) return lit->var;
				return NULL;
			}
			case BINARY: {
				ASTBinaryExpr* binary = (ASTBinaryExpr*) node;
				ASTVar* base = GetVar(binary->left);
				ASTVar* prop = GetVar(binary->right);
				ASTObjectInstance* inst = NULL;
				if (base == NULL || (base->value != NULL && base->value->type != OBJECT_INSTANCE)){ // TODO: confirm in all instances
					inst = GetCurrentObject();
				}
				else{
					inst = (ASTObjectInstance*) base->value;
				}
				SetRowCol(binary->left);
				if (inst == NULL) throw Error::INVALID_OBJECT;
				return inst->GetProp(isolate, prop->name);
			}
		}
		return NULL;
	}

	void Execute::SetCalc(ASTLiteral* lit){
		if (!lit->isCalc && lit->value.length() != 0 && (lit->litType == INT || lit->litType == DOUBLE || 
											 lit->litType == FLOAT /** || lit->litType == BOOLEAN**/)) {
			PrintStep("Set the calculated value (" + lit->value + " | " + Token::ToString(lit->litType) + ")");
			try{
				if (lit->value == "true" || lit->value == "false") lit->value = lit->value == "true" ? "1" : "0";
				lit->calc = std::stod(lit->value);
				lit->isCalc = true;
			}
			catch (...){
				SetRowCol(lit);
				throw Error::INVALID_CAST;
			}
		}
	}

	ASTLiteral* Execute::EvaluateValue(ASTNode* node){
		SetRowCol(node);
		if (node == NULL) return NULL;
		PrintStep("Evaluating Value (" + Token::ToString(node->type) + ")");
		int type = (int) node->type;
		switch (type){
			case UNDEFINED: {
				return (ASTLiteral*) node;
			}
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				ASTLiteral* value = EvaluateValue(lit->var);
				SetCast(lit, value);

				if (value != NULL && lit->unary != UNDEFINED){
					if (lit->unary == INC){
						value->calc++;
						value->value = std::to_string(value->calc);
						FormatLit(value);
					}
					else if (lit->unary == DEC){
						value->calc--;
						value->value = std::to_string(value->calc);
						FormatLit(value);
					}
				}

				if (value != NULL) return value;
				SetCalc(lit);
				return lit;
			}
			case VAR: {
				ASTVar* var = (ASTVar*) node;
				if (var->local != NULL) return var->local; // check this if this is always true
				return EvaluateValue(var->value);
			}
			case FUNC_CALL: {
				return EvaluateFuncCall((ASTFuncCall*) node);
			}
			case BINARY: {
				return EvaluateBinary((ASTBinaryExpr*) node);
			}
			case OBJECT_INSTANCE: {
				return (ASTLiteral*) node;
			}
		}
		return NULL;
	}

	ASTLiteral* Execute::EvaluateVar(ASTVar* var){
		PrintStep("Evaluating Var (" + var->name + ")");
		SetRowCol(var);
		Trace("Evaluating Var", var->name);
		if (var->name == "age2"){
			int a = 10; // here
		}
		ASTLiteral* local = (ASTLiteral*) EvaluateValue(var);
		if (var->previouslyDeclared && var->op == ASSIGN){
			if (var->local != NULL) isolate->RunGC(var, false);
		}
		if (local->type == OBJECT_INSTANCE){ // call constructor
			ASTObjectInstance* inst = (ASTObjectInstance*) local;
			if (inst->base != NULL) PrintStep("Calling Constructor for " + inst->base->name);
			if (inst->constructor != NULL && !inst->constructorCalled){
				AddObject(inst);
				EvaluateFuncCall(inst->constructor);
				RemoveObject();
				inst->constructorCalled = true;
			}
		}
		var->local = local->Clone(isolate, true);
		return local;
	}

	void Execute::EvaluateFor(ASTForExpr* expr){
		PrintStep("Evaluating For Loop");
		SetRowCol(expr);
		Trace("Evaluating", "For");
		ASTVar* var = (ASTVar*) expr->var;
		ASTLiteral* init = EvaluateVar(var);
		var->local = init;
		OpenScope(expr->scope);
		while (true){
			ASTLiteral* condition = EvaluateValue(expr->condition);
			bool pass = condition->value == "true";
			if (condition != expr->condition) isolate->RunGC(condition, true);
			if (!pass) break;
			bool cb = canBreak;
			canBreak = true;
			try {
				Evaluate();
				isolate->RunGC(expr->scope, true);
			}
			catch (Error::ERROR e){
				canBreak = cb;
				if (e == Error::INTERNAL_BREAK) {
					isolate->RunGC(expr->scope, true);
					break;
				}
				throw e;
			}
			canBreak = cb;
			OpenScope(expr->scope);
			var->local = EvaluateValue(expr->tick);	
		}
		CloseScope();
	}

	void Execute::EvaluateWhile(ASTWhileExpr* expr){
		PrintStep("Evaluating While Loop");
		SetRowCol(expr);
		Trace("Evaluating", "While");
		OpenScope(expr->scope);
		while (true){
			ASTLiteral* condition = EvaluateValue(expr->condition);
			bool pass = condition->value == "true";
			if (condition != expr->condition) {
				condition->Free(isolate); // release condition memory
				isolate->RunGC(expr->condition, true);
			}
			if (!pass) break;
			bool cb = canBreak;
			canBreak = true;
			try {
				Evaluate();
				isolate->RunGC(expr->scope, true);
			}
			catch (Error::ERROR e){
				canBreak = cb;
				if (e == Error::INTERNAL_BREAK) {
					isolate->RunGC(expr->scope, true);
					break;
				}
				throw e;
			}
			canBreak = cb;
			OpenScope(expr->scope);
		}
		CloseScope();
	}

	bool Execute::EvaluateIf(ASTIf* expr){
		if (expr == NULL) return false;
		PrintStep("Evaluating If Stmt");
		SetRowCol(expr);
		Trace("Evaluating","If");
		ASTLiteral* condition = EvaluateValue(expr->condition);
		bool pass = condition == NULL || condition->value == "true";
		if (condition != NULL && condition != expr->condition) {
			condition->Free(isolate); // release condition memory
			isolate->RunGC(expr->condition, false);
		}
		if (pass){
			OpenScope(expr->scope);
			try {
				Evaluate();
				isolate->RunGC(expr->scope, true);
			}
			catch (Error::ERROR e){
				if (e == Error::INTERNAL_BREAK){
					isolate->RunGC(expr->scope, true);
					throw e;
				}
			}
			return true;
		}
		else{
			if (condition != expr->condition) condition->Free(isolate); // release condition memory
			for (int i = 0; i < expr->elseIfs.size(); i++){
				if (EvaluateIf(expr->elseIfs[i])) break;
			}
		}
		return false;
	}

	void Execute::EvaluateSwitch(ASTSwitch* expr){
		PrintStep("Evaluating Switch Stmt");
		SetRowCol(expr);
		Trace("Evaluating", "Switch");
		ASTLiteral* value = EvaluateValue(expr->value);
		for (int i = 0; i < expr->cases.size(); i++){
			ASTLiteral* condition = EvaluateValue(expr->cases[i]->condition);
			ASTCase* stmt = expr->cases[i];
			SetRowCol(stmt);
			if (condition == NULL && !stmt->isDefault) throw Error::INVALID_CASE_STMT;
			if (stmt->isDefault || condition->value == value->value){
				if (condition != stmt->condition) isolate->RunGC(condition, true);
				if (!stmt->scope->IsParsed()) {
					stmt->scope->outer = GetCurrentScope();
					stmt->scope = semantic->ParseAndScan(stmt->scope);
				}
				OpenScope(stmt->scope);
				bool cb = canBreak;
				canBreak = true;
				try {
					Evaluate();
					isolate->RunGC(stmt->scope, true);
				}
				catch (Error::ERROR e){
					canBreak = cb;
					if (e == Error::INTERNAL_BREAK) {
						isolate->RunGC(stmt->scope, true);
						return;
					}
					throw e;
				}
				canBreak = cb;
				return;
			}
			if (condition != stmt->condition) condition->Free(isolate);
		}
	}

} // namespace internal
} // namespace Cobra