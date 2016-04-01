#include "execute.h"

namespace Cobra {
namespace internal{

	Execute* Execute::New(Isolate* isolate, Scope* scope){
		void* pt = isolate->GetMemory(sizeof(Execute));
		Execute* n = new(pt) Execute(scope);
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
			}
		}
	}

	void Execute::EvaluateBinary(ASTBinaryExpr* binary){
		ASTFuncCall* call = (ASTFuncCall*) binary->right;
		ASTLiteral* lit = (ASTLiteral*) call->params[0];
		//call->ptr(lit->value);
		//printf("d: %d\n", call->ptr);
	}

} // namespace internal
} // namespace Cobra