#include "exception.h"

namespace Cobra {
namespace internal{

	void Exception::CB(Isolate* isolate){
		Script::RunInternalScript(isolate, ExceptionBytes, "exception");
	}

} // namespace internal
}

