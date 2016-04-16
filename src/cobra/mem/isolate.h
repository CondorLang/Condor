#ifndef ISOLATE_H_
#define ISOLATE_H_

#include "allocate.h"
#include "cobra/ast/context.h"
#include "cobra/ast/scope.h"
#include "cobra/ast/node.h"

namespace Cobra {
namespace internal{

	class Context;
	class GC;

	class Isolate
	{
	private:
		MemoryPool* medium;
		MemoryPool* large;
		MemoryPool* small;
		MemoryPool* xl;
		MemoryPool* self;
		GC* gc;
		Context* context;

	public:
		Isolate();
		~Isolate();
		Context* CreateContext();
		Context* GetContext(){return context;}
		void* GetMemory(const size_t size);
		void FreeMemory(void* ptr, const size_t size);
		void SetSelfPool(MemoryPool* mp){this->self = mp;}
		void Dispose();
		void RunGC(Scope* scope);
		void RunGC(ASTNode* node);
		
	};

} // namespace internal
} // namespace Cobra

#endif // ISOLATE_H_