#include "date.h"

namespace Cobra {
namespace internal{

	void Date::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, DateBytes, "date", sub);
	}

} // namespace internal
}