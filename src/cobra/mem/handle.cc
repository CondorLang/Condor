#include "handle.h"
#include "cobra/types/string.h"

namespace Cobra{
namespace internal{

	const char* Handle::ToString(){
			if (obj->type == SCRIPT){
				Script* script = CAST(Script*, obj->address);
				Handle* src = script->GetSource();
				if (src->GetObject()->type == STRING){
					String* source = CAST(String*, src->GetObject()->address);
					return source->GetValue();
				}
			}
			else if (obj->type == STRING){
				String* source = CAST(String*, obj->address);
				return source->GetValue();
			}

			return "";
		}

} // namespace internal
} // namespace Cobra