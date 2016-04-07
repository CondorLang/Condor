#include "console.h"

namespace Cobra {
namespace internal{

	void Console::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ConsoleBytes, "console", sub);
	}

} // namespace internal
}