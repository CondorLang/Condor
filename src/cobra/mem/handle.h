#ifndef HANDLE_H_
#define HANDLE_H_

#include "heap.h"
#include "cobra/types/script.h"

namespace Cobra {
namespace internal{

	class Script;

	class Handle
	{
	private:
		HeapObject* obj;
	public:
		Handle(HeapObject* o) : obj(o){}
		~Handle(){}
		HeapObject* GetObject(){return obj;}
		const char* ToString();
	};

} // namespace internal
} // namespace Cobra

#endif // HANDLE_H_