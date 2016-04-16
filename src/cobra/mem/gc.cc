#include "gc.h"

namespace Cobra {
namespace internal{

	GC::GC(){

	}

	void GC::Run(MemoryPool* pool){
		pool->FreeUnnecessaryMemory();
	}

	void GC::Dispose(Isolate* isolate, Scope* scope){
		for (int i = 0; i < scope->Size(); i++){
			Dispose(isolate, scope->Get(i));
		}
	}

	void GC::Dispose(Isolate* isolate, ASTNode* node){
		if (node != NULL && node->local != NULL) node->local->Free(isolate);
	}

} // namespace internal
} // namespace Cobra
