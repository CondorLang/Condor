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
		return Calculate();
		return NULL;
	}

	// TODO: Implement paranthesis off of binary->isInParen
	void Execute::FillPostix(ASTBinaryExpr* binary){
		ASTLiteral* left = EvaluateValue(binary->left);
		ASTToken* tok = ASTToken::New(isolate, binary->op);

		if (left != NULL) stack.push_back(left);
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
	}

	// TODO: Crawl through the stack and calculate the result
	ASTLiteral* Execute::Calculate(){

	}

	ASTLiteral* Execute::EvaluateValue(ASTNode* node){
		SetRowCol(node);
		if (node == NULL) return NULL;
		int type = (int) node->type;
		switch (type){
			case LITERAL: {
				ASTLiteral* lit = (ASTLiteral*) node;
				ASTLiteral* value = EvaluateValue(lit->var);
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
		return (ASTLiteral*) EvaluateValue(var->value);
	}

	// TODO: Complete for loop
	void Execute::EvaluateFor(ASTForExpr* expr){
		SetRowCol(expr);
		Trace("Evaluating", "For");
		ASTLiteral* init = EvaluateVar((ASTVar*) expr->var);
		int value = init->To<int>();
		// condition will be hard coded for now
		while (true){
			if (value >= 10) break; // condition
			OpenScope(expr->scope);
			Evaluate();
			value++; // increment
		}
	}

} // namespace internal
} // namespace Cobra