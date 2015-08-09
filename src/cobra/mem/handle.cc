#include "handle.h"

namespace Cobra{
namespace internal{

	String* Handle::ToString(){
		return CAST(String*, obj->address);
	}

	Script* Handle::ToScript(){
		return CAST(Script*, obj->address);
	}

} // namespace internal
} // namespace Cobra