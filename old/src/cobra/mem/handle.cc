#include "handle.h"

namespace Cobra{
namespace internal{

	String* Handle::ToString(){
		return CAST(String*, obj->address);
	}

	Script* Handle::ToScript(){
		return CAST(Script*, obj->address);
	}

	bool Handle::IsAddressValid(Isolate* iso, Address addr){
		return iso->IsAddressValid(addr);
	}

	Handle* Handle::New(HeapObject* o, Isolate* iso){
		void* p = iso->GetMemory(sizeof(Handle));
		Handle* h = new(p) Handle(o, iso);
		return h;
	}

} // namespace internal
} // namespace Cobra