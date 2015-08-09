#ifndef HANDLE_H_
#define HANDLE_H_

#include "heap.h"
#include "cobra/types/script/script.h"
#include "cobra/types/strings/string.h"

namespace Cobra {
namespace internal{

	class Script;
	class String;

	class Handle
	{
	private:
		HeapObject* obj;
	public:
		Handle(HeapObject* o) : obj(o){}
		~Handle(){}
		HeapObject* GetObject(){return obj;}
		String* ToString();
		Script* ToScript();
	};

} // namespace internal
} // namespace Cobra

#endif // HANDLE_H_