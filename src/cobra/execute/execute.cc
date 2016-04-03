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
		if (trace) printf("%s - %s\n", first.c_str(), msg2.c_str());
	}

	void Execute::OpenScope(Scope* sc){
		scopes.insert(scopes.begin(), sc);
	}

	void Execute::Evaluate(){
		Scope* scope = GetCurrentScope();
		if (scope == NULL) return; // nothing to evaluate
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			int type = (int) node->type;
			switch (type){
				case BINARY: EvaluateBinary((ASTBinaryExpr*) node); break;
				case FUNC_CALL: EvaluateFuncCall((ASTFuncCall*) node); break;
			}
		}
	}

	// TODO: Run a GC to free memory for "true" local variables and clear the rest.
	void Execute::Empty(Scope* scope){
		scope->local.clear();
	}

	void Execute::EvaluateBinary(ASTBinaryExpr* binary){
		if (binary->right->type == FUNC_CALL && ((ASTFuncCall*) binary->right)->func->HasVisibility(STATIC)){
			Trace("Evaluate", "Static func call - " + binary->right->name);
			EvaluateFuncCall((ASTFuncCall*) binary->right);
		}
	}

	void Execute::EvaluateFuncCall(ASTFuncCall* call){
		if (call->isInternal && call->callback != nullptr) {
			ASTLiteral* lit = (ASTLiteral*) call->params[0];
			lit = ExtractLiteral(lit);
			return CALL_INTERNAL(isolate, call->callback, lit);
		}

		ASTFunc* func = call->func;
		Scope* s = func->scope;
		Empty(s);
		for (int i = 0; i < call->params.size(); i++){
			s->local.push_back(call->params[i]);
		}
		OpenScope(s);
		Evaluate();
	}

	// TODO: Scrap this function, too many flaws. 
	ASTLiteral* Execute::ExtractLiteral(ASTLiteral* lit){
		if (lit->var == NULL) return lit;
		ASTVar* var = lit->var;
		if (var->isArg && var->order > -1){
			return (ASTLiteral*) GetCurrentScope()->local.at(var->order);
		}
		return NULL;
	}

} // namespace internal
} // namespace Cobra