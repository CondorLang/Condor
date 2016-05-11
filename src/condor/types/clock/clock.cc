// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "clock.h"

#include "condor/clock.h"

namespace Condor {
namespace internal{

	CBClockContainer* CBClockContainer::New(Isolate* iso){
		void* p = (CBClockContainer*) iso->GetMemory(sizeof(CBClockContainer));
		CBClockContainer* container = new(p) CBClockContainer(iso);
		return container;
	}

	CBClockContainer* CBClockContainer::GlobalClockContainer = NULL;

	int CBClockContainer::AddClock(){
		Clocks.push_back(Clock::New(isolate)); return (int)Clocks.size();
	}

} // namespace internal
}