#include "gc.h"

namespace Cobra {
namespace internal{

	GC::GC(){
	}

	void GC::Run(MemoryPool* pool){
		pool->FreeUnnecessaryMemory();
	}

	GC* GC::New(Isolate* isolate){
		void* p = isolate->GetMemory(sizeof(GC));
		GC* gc = new(p) GC();
		return gc;
	}

	bool GC::InParentScopes(int id){
		for (int i = 1; i < scopes.size(); i++){
			if (scopes[i]->scopeId == id) return true;
		}
		return false;
	}

	void GC::Dispose(Isolate* isolate, Scope* scope, bool deep){
		OpenScope(scope);
		for (int i = 0; i < scope->Size(); i++){
			Dispose(isolate, scope->Get(i), deep);
		}
		CloseScope();
	}

	void GC::Dispose(Isolate* isolate, ASTNode* node, bool deep){
		if (node == NULL) return;
		if (node->allowGC && node->local != NULL) {
			node->local->Free(isolate);
			node->local = NULL;
		}
		if (deep){
			int type = (int) node->type;
			switch (type){
				case BINARY: {
					ASTBinaryExpr* expr = (ASTBinaryExpr*) node;
					Dispose(isolate, expr->left, true);
					Dispose(isolate, expr->right, true);
					break;
				}
				case FUNC: {
					ASTFunc* func = (ASTFunc*) node;
					Dispose(isolate, func->scope, true);
					for (int i = 0; i < func->args.size(); i++){
						Dispose(isolate, func->args[i], true);
					}
					break;
				}
				case OBJECT_INSTANCE: {
					ASTObjectInstance* inst = (ASTObjectInstance*) node;
					
					break;
				}
				case VAR: {
					ASTVar* var = (ASTVar*) node;
					Dispose(isolate, var->value, true);
					Dispose(isolate, var->member, true);				
					break;
				}
				case FUNC_CALL: {
					ASTFuncCall* call = (ASTFuncCall*) node;
					if (call->func == NULL) return;
					for (int i = 0; i < call->params.size(); i++){
						if (i < call->func->args.size() && call->params[i] == call->func->args[i]->local){
							call->func->args[i]->allowGC = false;
						}
					}
					Dispose(isolate, call->func, true);
					break;
				}
				case OBJECT: case IF: { // handled in execute.cc
					break;
				}
				case LITERAL: {
					ASTLiteral* lit = (ASTLiteral*) node;
					if (lit->allowGC) Dispose(isolate, lit->var, true);
					break;
				}
				default: {
					printf("d: %s\n", Token::ToString(node->type).c_str());
				}
			}
		}
	}

} // namespace internal
} // namespace Cobra
