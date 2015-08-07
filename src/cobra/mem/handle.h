#ifndef HANDLE_H_
#define HANDLE_H_

#include "cobra/mem/heap.h"

namespace Cobra {
namespace internal{

	struct HeapObject;

	class Handle
	{
	private: 
		HeapObject* obj;
	public:
		Handle(HeapObject* o) : obj(o){}
		~Handle();
	};

} // namespace internal
} // namespace Cobra

#endif // HANDLE_H_