#ifndef ISOLATE_H_
#define ISOLATE_H_

/**
 * The Isolate class is an isolated instance of Cobra. Memory allocations,
 * heaps, AST, and base code will be isolated.
 */

#include "heap.h"
#include "cobra/assert.h"

namespace Cobra {
namespace internal{

	class Isolate
	{
	private:
		HeapStore heapstore;
	public:
		Isolate(){}
		~Isolate();
		void Enter();
		void Exit();
		HeapObject* Insert(HeapObject obj);
	};

} // namespace internal
} // namespace Cobra

#endif // ISOLATE_H_