#include "exception.h"

namespace Cobra {
namespace internal{

	void Exception::CB(Isolate* isolate){
		Script::RunInternalScript(isolate, ExceptionBytes);
	}

} // namespace internal
}

