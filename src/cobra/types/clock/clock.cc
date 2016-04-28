#include "clock.h"

namespace Cobra {
namespace internal{

	void CBClock::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ClockBytes, "clock", sub);
	}

	CBClockContainer* CBClockContainer::New(Isolate* iso){
		void* p = (CBClockContainer*) iso->GetMemory(sizeof(CBClockContainer));
		CBClockContainer* container = new(p) CBClockContainer(iso);
		return container;
	}

	CBClockContainer* CBClockContainer::GlobalClockContainer = NULL;

} // namespace internal
}