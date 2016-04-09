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
		row = -1;
		col = -1;
	}

	void Execute::Trace(std::string first, std::string msg2){
		if (trace) {
			std::string tabs = "";
			for (int i = 0; i < scopes.size(); i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first.c_str(), msg2.c_str());
		}
	}

	void Execute::SetRowCol(ASTNode* node){
		if (node == NULL) return;
		row = node->row;
		col = node->col;
	}

	void Execute::OpenScope(Scope* sc){
		scopes.insert(scopes.begin(), sc);
	}

	// TODO: Erase values in all non return locals
	void Execute::Evaluate(){
		Scope* scope = GetCurrentScope();
		if (scope == NULL) return; // nothing to evaluate
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			int type = (int) node->type;
			switch (type){
				case FUNC_CALL: node->local = EvaluateFuncCall((ASTFuncCall*) node); break;
				case BINARY: node->local = EvaluateBinary((ASTBinaryExpr*) node); break;
				case VAR: node->local = EvaluateVar((ASTVar*) node); break;
				case FOR: EvaluateFor((ASTForExpr*) node); break;
				case WHILE: EvaluateWhile((ASTWhileExpr*) node); break;
				case IF: EvaluateIf((ASTIf*) node); break;
				case LITERAL: node->local = EvaluateValue(node); break;
			}
		}
		CloseScope();
	}

	ASTLiteral* Execute::EvaluateFuncCall(ASTFuncCall* call){
		SetRowCol(call);
		Trace("Evaluating Func Call", call->name);
		ASTFunc* func = call->func;
		if (func == NULL && call->isInternal){ // internal
			ASTLiteral* lit = NULL;
			if (call->params.size() > 0) lit = EvaluateValue(call->params[0]);
			return (ASTLiteral*) Internal::CallInternal(isolate, call->callback, lit);
		}
		else{
			if (func == NULL) throw Error::UNDEFINED_FUNC;
			for (int i = 0; i < func->args.size(); i++){
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
		SetRowCol(binary);
		if (binary->op == PERIOD && binary->right->type == FUNC_CALL && ((ASTFuncCall*) binary->right)->func->HasVisibility(STATIC)){
			return (ASTLiteral*) EvaluateFuncCall((ASTFuncCall*) binary->right);
		}
		FillPostix(binary);
		ASTLiteral* lit = Calculate();
		FormatLit(lit);
		stack.clear();
		opStack.clear();
		return lit;
	}

	void Execute::SetCast(ASTExpr* expr, ASTLiteral* value){
		if (value == NULL) return;
		if (expr->cast != NULL){
			ASTLiteral* lit = expr->cast;
			value->litType = lit->litType;
			FormatLit(value, true);
		}
	}

	void Execute::FormatLit(ASTLiteral* lit, bool forceType){
		if (lit == NULL) return;
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
		ASTLiteral* left = EvaluateValue(binary->left);
		ASTToken* tok = ASTToken::New(isolate, binary->op);

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
		else stack.push_back(EvaluateValue(binary->right));
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
		for (int i = opStack.size() - 1; i >= 0; i--){
			stack.push_back(opStack[i]);
			opStack.pop_back();
		}

		// opStack is now the stack
		for (int i = 0; i < stack.size(); i++){
			ASTNode* n = stack[i];
			if (n->type == TOK){
				ASTToken* tok = (ASTToken*) n;
				ASTLiteral* n = Calc(tok);
				if (n != NULL) opStack.push_back(n);
			}
			else if (n->type == LITERAL){
				ASTLiteral* lit = (ASTLiteral*) n;
				lit = EvaluateValue(n);
				lit->calc = std::stod(lit->value);
				opStack.push_back(lit);
			}
			else{
				printf("d: %s\n", "Unknown");
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
		ASTLiteral* second = (ASTLiteral*) opStack[opStack.size() - 1];
		opStack.pop_back();
		ASTLiteral* result = ASTLiteral::New(isolate);
		result->litType = Binary::Compare(first->litType, second->litType, tok->value->value);
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
		}
		return result;
	}

	ASTLiteral* Execute::EvaluateValue(ASTNode* node){
		SetRowCol(node);
		if (node == NULL) return NULL;
		int type = (int) node->type;
		switch (type){
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
				return (ASTLiteral*) lit;
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
		}
		return NULL;
	}

	ASTLiteral* Execute::EvaluateVar(ASTVar* var){
		SetRowCol(var);
		Trace("Evaluating Var", var->name);
		ASTLiteral* local = (ASTLiteral*) EvaluateValue(var->value);
		if (var->previouslyDeclared && var->op == ASSIGN){
			if (var->local != NULL) var->local->Free(isolate);
		}
		return local;
	}

	void Execute::EvaluateFor(ASTForExpr* expr){
		SetRowCol(expr);
		Trace("Evaluating", "For");
		ASTVar* var = (ASTVar*) expr->var;
		ASTLiteral* init = EvaluateVar(var);
		var->local = init;
		OpenScope(expr->scope);
		// condition will be hard coded for now
		while (true){
			ASTLiteral* condition = EvaluateValue(expr->condition);
			if (condition->value == "false") break; // condition
			condition->Free(isolate); // release condition memory
			Evaluate();
			OpenScope(expr->scope);
			var->local = EvaluateValue(expr->tick);	
		}
		CloseScope();
	}

	void Execute::EvaluateWhile(ASTWhileExpr* expr){
		SetRowCol(expr);
		Trace("Evaluating", "While");
		OpenScope(expr->scope);
		while (true){
			ASTLiteral* condition = EvaluateValue(expr->condition);
			if (condition->value == "false") break; // condition
			condition->Free(isolate); // release condition memory
			Evaluate();
			OpenScope(expr->scope);
		}
		CloseScope();
	}

	bool Execute::EvaluateIf(ASTIf* expr){
		SetRowCol(expr);
		Trace("Evaluating","If");
		ASTLiteral* condition = EvaluateValue(expr->condition);
		if (condition->value == "true"){
			condition->Free(isolate); // release condition memory
			OpenScope(expr->scope);
			Evaluate();
			return true;
		}
		else{
			condition->Free(isolate); // release condition memory
			for (int i = 0; i < expr->elseIfs.size(); i++){
				if (EvaluateIf(expr->elseIfs[i])) break;
			}
		}
		return false;
	}

} // namespace internal
} // namespace Cobra