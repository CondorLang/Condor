#include "app.h"

namespace Cobra {
namespace internal{

	void App::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, AppBytes, "app", sub);
	}

	bool App::Included = false;

} // namespace internal
}