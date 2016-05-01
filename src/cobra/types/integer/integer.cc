#include "integer.h"

namespace Cobra {
namespace internal{

	void Integer::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, IntegerBytes, "integer", sub);
	}

} // namespace internal
}