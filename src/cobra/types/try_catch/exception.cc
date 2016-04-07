#include "exception.h"

namespace Cobra {
namespace internal{

	void Exception::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ExceptionBytes, "exception", sub);
	}

} // namespace internal
}

