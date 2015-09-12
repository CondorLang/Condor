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

} // namespace internal
} // namespace Cobra