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
	}

	void Execute::Trace(std::string first, std::string msg2){
		if (trace) {
			std::string tabs = "";
			for (int i = 0; i < scopes.size(); i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first.c_str(), msg2.c_str());
		}
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
			}
		}
		CloseScope();
	}

	ASTLiteral* Execute::EvaluateFuncCall(ASTFuncCall* call){
		Trace("Evaluating Func Call", call->name);
		ASTFunc* func = call->func;
		if (func == NULL && call->isInternal){ // internal
			ASTLiteral* lit = NULL;
			if (call->params.size() > 0) lit = EvaluateValue(call->params[0]);
			return (ASTLiteral*) Internal::CallInternal(isolate, call->callback, lit);
		}
		else{
			for (int i = 0; i < func->args.size(); i++){
				func->args[i]->value = EvaluateValue(call->params[i]);
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
		if (binary->right->type == FUNC_CALL && ((ASTFuncCall*) binary->right)->func->HasVisibility(STATIC)){
			return (ASTLiteral*) EvaluateFuncCall((ASTFuncCall*) binary->right);
		}
		return NULL;
	}

	ASTLiteral* Execute::EvaluateValue(ASTNode* node){
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
		Trace("Evaluating Var", var->name);
		return (ASTLiteral*) EvaluateValue(var->value);
	}

} // namespace internal
} // namespace Cobra