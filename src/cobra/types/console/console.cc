#include "console.h"

namespace Cobra {
namespace internal{

	void Console::CB(Isolate* isolate){
		Script::RunInternalScript(isolate, ConsoleBytes, "console");
	}

} // namespace internal
}