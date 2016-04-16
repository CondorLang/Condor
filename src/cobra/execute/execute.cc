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
		if (scope == NULL) return; // nothing to evaluate
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			int type = (int) node->type;
			switch (type){
				case FUNC_CALL: node->local = EvaluateFuncCall((ASTFuncCall*) node); break;
				case BINARY: EvaluateBinary((ASTBinaryExpr*) node); break;
				case VAR: node->local = EvaluateVar((ASTVar*) node); break;
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
			if (call->params.size() > 0) lit = EvaluateValue(call->params[0]);
			return (ASTLiteral*) Internal::CallInternal(isolate, call->callback, lit);
		}
		else{
			if (func == NULL) throw Error::UNDEFINED_FUNC;
			for (int i = 0; i < func->args.size(); i++){
				PrintStep("Evaluating Parameter (" + func->args[i]->name + ")");
				if (call->params.size() > i) func->args[i]->value = EvaluateValue(call->params[i]);
				else{
					func->args[i]->value = ASTUndefined::New(isolate);
				}
			}
			OpenScope(func->scope);
			Evaluate();
			for (int i = 0; i < func->args.size(); i++){
				func->args[i]->value->Free(isolate);
			}
		}
		std::vector<ASTNode*> returns = func->scope->Lookup("return", false);
		if (returns.size() > 0) return (ASTLiteral*) returns[0]->local;
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
				FillPostix(binary);
				ASTLiteral* lit = Calculate();
				FormatLit(lit);
				stack.clear();
				opStack.clear();
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
		ASTLiteral* left = NULL;
		if (postixPeriod){
			left = (ASTLiteral*) binary->left;
		}
		else{
			left = EvaluateValue(binary->left);
		}
		ASTToken* tok = ASTToken::New(isolate, binary->op);
		postixPeriod = binary->op == PERIOD;

		if (left != NULL) stack.push_back(left);
		if (binary->isInParen) opStack.push_back(ASTToken::New(isolate, LPAREN));
		if (opStack.size() == 0) opStack.push_back(tok);
		else if (tok->value->value == LPAREN) opStack.push_back(tok);
		else if (tok->value->value == RPAREN){
			for (int i = opStack.size() - 1; i >= 0; i--){
				ASTToken* t = (ASTToken*) opStack[i];
				if (t->value->value == LPAREN) break;
				else{
					stack.push_back(t);
					opStack.pop_back();
				}
				opStack.pop_back();
			}
		}
		else if (((ASTToken*)(opStack[opStack.size() - 1]))->Precedence() < tok->Precedence()) opStack.push_back(tok);
		else if (((ASTToken*)(opStack[opStack.size() - 1]))->Precedence() >= tok->Precedence()){
			for (int i = opStack.size() - 1; i >= 0; i--){
				ASTToken* t = (ASTToken*) opStack[i];
				if (t->Precedence() < tok->Precedence()) break;
				else{
					stack.push_back(t);
					opStack.pop_back();
				}
			}
			opStack.push_back(tok);
		}
		if (binary->right != NULL && binary->right->type == BINARY) FillPostix((ASTBinaryExpr*) binary->right);
		else {
			postixPeriod = false;
			if (binary->op == PERIOD) stack.push_back(binary->right);
			else stack.push_back(EvaluateValue(binary->right));
		}
		if (binary->isInParen) {
			for (int i = opStack.size() - 1; i >= 0; i--){
				ASTToken* t = (ASTToken*) opStack[i];
				if (t->value->value == LPAREN) break;
				else{
					stack.push_back(t);
					opStack.pop_back();
				}
			}
			opStack.pop_back();
		}
	}

	ASTLiteral* Execute::Calculate(){
		PrintStep("Evaluating Postix Values");
		for (int i = opStack.size() - 1; i >= 0; i--){
			stack.push_back(opStack[i]);
			opStack.pop_back();
		}

		std::vector<int> periods;
		for (int i = 0; i < stack.size(); i++){
			if (stack[i]->type == TOK){
				ASTToken* tok = (ASTToken*) stack[i];
				if (tok->value->value == PERIOD) periods.push_back(i); // push index
			}
		}

		// --rpn-stack flag
		if (rpnStack){
			printf("\nStack Size: %lu\n", stack.size());
			for (int i = 0; i < stack.size(); i++){
				ASTNode* n = stack[i];
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
				else{
					printf("dd: %s\n", Token::ToString(n->type).c_str());
				}
			}
		}

		if (rpnStack) printf("\n--- Evaluating ---\n");

		// opStack is now the stack
		for (int i = 0; i < stack.size(); i++){
			ASTNode* n = stack[i];

			if (n->type == TOK){
				ASTToken* tok = (ASTToken*) n;
				if (rpnStack) printf("[%d]: %s\n", i, tok->value->String().c_str());
				ASTLiteral* val = Calc(tok);
				if (val != NULL) {
					opStack.push_back(val);
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
					opStack.push_back(lit);
				}
				else{
					if (rpnStack) printf("[%d]: %s (g)\n", i, ((ASTLiteral*)n)->value.c_str());
					lit = EvaluateValue(n);
					SetCalc(lit);
					opStack.push_back(lit);
				}
			}
			else if (n->type == OBJECT_INSTANCE){ // TODO: What about twice?
				ASTObjectInstance* lit = (ASTObjectInstance*) n;
				if (rpnStack) printf("[%d]: obj(%s)\n", i, lit->base->name.c_str());
				opStack.push_back(lit);
			}
			else{
				printf("dd: %s\n", Token::ToString(n->type).c_str());
			}
		}
		if (opStack.size() == 0) throw Error::INVALID_OPERATOR;
		return (ASTLiteral*) opStack[0];
	}

	ASTLiteral* Execute::Calc(ASTToken* tok){
		SetRowCol(tok);
		int type = (int) tok->value->value;
		if (opStack.size() < 2) throw Error::INVALID_OPERATOR;
		ASTLiteral* first = (ASTLiteral*) opStack[opStack.size() - 1];
		opStack.pop_back();
		if (tok->value->IsAssign()) return first;
		ASTLiteral* second = (ASTLiteral*) opStack[opStack.size() - 1];
		opStack.pop_back();
		ASTLiteral* result = ASTLiteral::New(isolate);

		result->litType = Binary::Compare(first->litType, second->litType, tok->value->value);

		PrintStep("Calculating ('" + second->value + "' " + tok->value->String() + " '" + first->value + "')");

		int litType = (int) result->litType;
		switch (litType){
			case LITERAL: { // object oriented
				ASTObjectInstance* inst = (ASTObjectInstance*) second;
				ASTVar* var = inst->GetProp(isolate, first->value);
				ASTLiteral* val = EvaluateValue(var);
				return val;
			}
			case INT: case DOUBLE: 
			case FLOAT: case BOOLEAN: {
				switch (type){
					case ADD:	result->calc = second->calc + first->calc; break;
					case DIV:	result->calc = second->calc / first->calc; break;
					case MUL:	result->calc = second->calc * first->calc; break;
					case SUB:	result->calc = second->calc - first->calc; break;
					case MOD:	result->calc = (int) second->calc % (int) first->calc; break;
					case LOR:	result->calc = second->calc == 0 || first->calc == 0; break;
					case LAND:	result->calc = second->calc != 0 && first->calc != 0; break;
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
				break;
			}
			case STRING: case CHAR: {
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
				break;
			}
		}
		return result;
	}

	// TODO: Implement setting object variables
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
				if (var->local != NULL) var->local->Free(isolate);
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
				ASTObjectInstance* inst = (ASTObjectInstance*) base->value;
				SetRowCol(binary->left);
				if (inst == NULL) throw Error::INVALID_OBJECT;
				return inst->GetProp(isolate, prop->name);
			}
		}
		return NULL;
	}

	void Execute::SetCalc(ASTLiteral* lit){
		PrintStep("Set the calculated value (" + lit->value + " | " + Token::ToString(lit->litType) + ")");
		if (!lit->isCalc && lit->value.length() != 0 && (lit->litType == INT || lit->litType == DOUBLE || 
											 lit->litType == FLOAT /** || lit->litType == BOOLEAN**/)) {
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
				ASTLiteral* l = (ASTLiteral*) lit;
				SetCalc(l);
				return l;
			}
			case VAR: {
				ASTVar* var = (ASTVar*) node;
				if (var->local != NULL) return (ASTLiteral*) var->local; // check this if this is always true
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
		ASTLiteral* local = (ASTLiteral*) EvaluateValue(var->value);
		if (var->previouslyDeclared && var->op == ASSIGN){
			if (var->local != NULL) var->local->Free(isolate);
		}
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
			if (condition->value != "true") break; // condition
			if (condition != expr->condition) condition->Free(isolate); // release condition memory
			bool cb = canBreak;
			canBreak = true;
			try {
				Evaluate();
			}
			catch (Error::ERROR e){
				canBreak = cb;
				if (e == Error::INTERNAL_BREAK) break;
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
			if (condition->value != "true") break; // condition
			if (condition != expr->condition) condition->Free(isolate); // release condition memory
			bool cb = canBreak;
			canBreak = true;
			try {
				Evaluate();
			}
			catch (Error::ERROR e){
				canBreak = cb;
				if (e == Error::INTERNAL_BREAK) break;
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
		if (condition == NULL || condition->value == "true"){
			if (condition != expr->condition) condition->Free(isolate); // release condition memory
			OpenScope(expr->scope);
			Evaluate();
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

	// TODO: Attach the correct parent scope to the scope
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
				if (condition != stmt->condition) condition->Free(isolate);
				if (!stmt->scope->IsParsed()) {
					stmt->scope->outer = GetCurrentScope();
					stmt->scope = semantic->ParseAndScan(stmt->scope);
				}
				OpenScope(stmt->scope);
				bool cb = canBreak;
				canBreak = true;
				try {
					Evaluate();
				}
				catch (Error::ERROR e){
					canBreak = cb;
					if (e == Error::INTERNAL_BREAK) return;
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