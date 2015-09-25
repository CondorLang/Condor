#ifndef ISOLATE_H_
#define ISOLATE_H_

/**
 * The Isolate class is an isolated instance of Cobra. Memory allocations,
 * heaps, AST, and base code will be isolated.
 */

#include "cobra/mem/heap.h"
#include "cobra/assert.h"
#include "cobra/globals.h"
#include "cobra/ast/context.h"
#include "cobra/flags.h"
#include "cobra/mem/factory.h"
#include "allocate.h"

namespace Cobra {
namespace internal{

	class Factory;
	class HeapStore;
	struct HeapObject;
	class Context;
	template<class T> class Pointer;
	class MemoryPool;

	class Isolate
	{
	private:
		HeapStore heapstore;
		void _enter();
		void _exit();
		Context* context;
		MemoryPool* mp;

	public:
		Isolate();
		~Isolate();
		Factory* factory;
		inline void Enter(){_enter();}
		inline void Exit(){_exit();}
		void* GetMemory(const size_t size);


		// Garbage
		HeapObject* Insert(HeapObject obj);

		template<class T>
		T* InsertToHeap(T* t, TOKEN type){
			HeapObject obj;
			obj.address = CAST(Address, t);
			obj.type = type;
			HeapObject* returnObj = Insert(obj);
			return CAST(T*, returnObj->address);
		}

		template<class T>
		Pointer<T> NewPointer(T* t, TOKEN type){
			HeapObject obj;
			obj.address = CAST(Address, t);
			obj.type = type;
			HeapObject* o = Insert(obj);
			Pointer<T> handle(this, o);
			return handle;
		}

		void FlushAST();
		void FlushAll();
		void SetContext(Context* context);
		Context* GetContext(){return context;}
		bool IsAddressValid(Address addr);
	};

} // namespace internal
} // namespace Cobra

#endif // ISOLATE_H_