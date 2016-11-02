// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "gc.h"

namespace Condor {
namespace internal{

	GC::GC(){
		printIds = PRINT_GC_IDS;
	}

	void GC::Run(MemoryPool* pool){
		
	}

	GC* GC::New(Isolate* isolate){
		void* p = isolate->GetMemory(sizeof(GC));
		GC* gc = new(p) GC();
		return gc;
	}

	bool GC::InParentScopes(int id){
		for (unsigned int i = 1; i < scopes.size(); i++){
			if (scopes[i]->scopeId == id) return true;
		}
		return false;
	}

	void GC::Clear(){
		cleansed.clear();
	}

	void GC::Dispose(Isolate* isolate, Scope* scope, bool deep, bool objKeys){
		OpenScope(scope);
		for (int i = 0; i < scope->Size(); i++){
			Dispose(isolate, scope->Get(i), deep, objKeys);
		}
		CloseScope();
	}

	void GC::Dispose(Isolate* isolate, ASTNode* node, bool deep, bool objKeys){
		if (node == NULL) return;
		if (cleansed.find(node->id) != cleansed.end()) return; // Already been cleansed
		if (node->allowGC && node->HasLocal()) {
			if (node->GetLocal(false)->type == OBJECT_INSTANCE){
				Dispose(isolate, node->GetLocal(false), deep, true);
			}
			node->GetLocal()->Free(isolate);
		}
		if (node->allowGC && printIds) printf("  %d\n", node->id);
		if (node->allowGC) cleansed[node->id] = true;
		if (deep){
			int type = (int) node->type;
			switch (type){
				case BINARY: {
					ASTBinaryExpr* expr = (ASTBinaryExpr*) node;
					Dispose(isolate, expr->left, true, objKeys);
					Dispose(isolate, expr->right, true, objKeys);
					break;
				}
				case FUNC: {
					ASTFunc* func = (ASTFunc*) node;
					for (unsigned int i = 0; i < func->args.size(); i++){
						Dispose(isolate, func->args[i], true, objKeys);
					}
					Dispose(isolate, func->scope, true, objKeys);
					break;
				}
				case OBJECT_INSTANCE: {
					ASTObjectInstance* inst = (ASTObjectInstance*) node;
					if (objKeys){
						for (std::map<std::string, ASTVar*>::iterator i = inst->properties.begin(); i != inst->properties.end(); ++i){
							Dispose(isolate, i->second, true, objKeys);
						}
					}
					break;
				}
				case VAR: {
					ASTVar* var = (ASTVar*) node;
					Dispose(isolate, var->value, true, objKeys);
					Dispose(isolate, var->member, true, objKeys);				
					break;
				}
				case FUNC_CALL: {
					ASTFuncCall* call = (ASTFuncCall*) node;
					if (call->func == NULL) return;
					for (unsigned int i = 0; i < call->params.size(); i++){
						if (i < call->func->args.size() && call->params[i] == call->func->args[i]->GetLocal(false)){
							call->func->args[i]->allowGC = false;
						}
					}
					Dispose(isolate, call->func, true, objKeys);
					break;
				}
				case OBJECT: case IF: case UNDEFINED: 
				case BREAK: case CONTINUE: { // handled in execute.cc
					break;
				}
				case LITERAL: {
					ASTLiteral* lit = (ASTLiteral*) node;
					if (lit->allowGC) Dispose(isolate, lit->var, true, objKeys);
					break;
				}
				case FOR: {
					ASTForExpr* expr = (ASTForExpr*) node;
					if (expr->allowGC) {
						Dispose(isolate, expr->var, true);
						Dispose(isolate, expr->condition, true);
						Dispose(isolate, expr->tick, true);
						Dispose(isolate, expr->scope, true);
					}
					break;
				}
				case WHILE: {
					ASTWhileExpr* expr = (ASTWhileExpr*) node;
					if (expr->allowGC){
						Dispose(isolate, expr->condition, true);
						Dispose(isolate, expr->scope, true);
					}
					break;
				}
				case ARRAY: {
					ASTArray* ary = (ASTArray*) node;
					for (unsigned int i = 0; i < ary->members.size(); i++){
						Dispose(isolate, ary->members[i], true);
					}
					break;
				}
				default: {
					printf("GC: %s\n", Token::ToString(node->type).c_str());
				}
			}
		}
	}

} // namespace internal
} // namespace Condor
