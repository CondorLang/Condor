#include "stack.h"

namespace Cobra {
namespace internal{

	RPNStack* RPNStack::New(Isolate* isolate){
		void* pt = isolate->GetMemory(sizeof(RPNStack));
		RPNStack* n = new(pt) RPNStack();
		return n;
	}

} // namespace internal
} // namespace Cobra