#include "types.h"

namespace Cobra {
namespace internal{

	void Types::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, TypesBytes, "type", sub);
	}

} // namespace internal
}