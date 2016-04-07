#include "array.h"

namespace Cobra {
namespace internal{

	void Array::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ArrayBytes, "array", sub);
	}

} // namespace internal
}