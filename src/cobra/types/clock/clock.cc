// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

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