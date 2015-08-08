#ifndef ISOLATE_H_
#define ISOLATE_H_

/**
 * The Isolate class is an isolated instance of Cobra. Memory allocations,
 * heaps, AST, and base code will be isolated.
 */

#include "cobra/mem/heap.h"
#include "cobra/assert.h"
#include "cobra/mem/factory.h"
#include "cobra/globals.h"

namespace Cobra {
namespace internal{

	class Factory;
	class HeapStore;
	struct HeapObject;

	class Isolate
	{
	private:
		HeapStore* heapstore;
		void _enter();
		void _exit();

	public:
		Isolate(){heapstore = new HeapStore();factory = new Factory();}
		~Isolate();
		Factory* factory;
		inline void Enter(){_enter();}
		inline void Exit(){_exit();}
		HeapObject* Insert(HeapObject obj);
	};

} // namespace internal
} // namespace Cobra

#endif // ISOLATE_H_