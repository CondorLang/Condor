#ifndef HANDLE_H_
#define HANDLE_H_

#include "heap.h"

namespace Cobra {
namespace internal{

	class Handle
	{
	private:
		HeapObject* obj;
	public:
		Handle(HeapObject* o) : obj(o){}
		~Handle(){}
		HeapObject* GetObject(){return obj;}
	};

} // namespace internal
} // namespace Cobra

#endif // HANDLE_H_