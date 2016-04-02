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
		this->scope = scope;
	}

	void Execute::Evaluate(){
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			int type = (int) node->type;
			switch (type){
				case BINARY: EvaluateBinary((ASTBinaryExpr*) node);
				case FUNC_CALL: EvaluateFuncCall((ASTFuncCall*) node);
			}
		}
	}

	void Execute::EvaluateBinary(ASTBinaryExpr* binary){

	}

	void Execute::EvaluateFuncCall(ASTFuncCall* call){
		if (call->isInternal && call->callback != nullptr) {
			ASTLiteral* lit = (ASTLiteral*) call->params[0];
			CALL_INTERNAL(isolate, call->callback, lit);
		}
	}

} // namespace internal
} // namespace Cobra