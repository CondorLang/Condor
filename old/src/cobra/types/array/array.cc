#include "array.h"

namespace Cobra {
namespace internal{

	void Array::CB(Isolate* isolate){
		Script::RunInternalScript(isolate, ArrayBytes, "array");
	}

} // namespace internal
}