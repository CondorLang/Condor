// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "clock.h"

namespace Cobra {
namespace internal{

	Clock* Clock::New(Isolate* isolate){
		void* p = (Clock*) isolate->GetMemory(sizeof(Clock));
		Clock* c = new(p) Clock();
		return c;
	}

	void Clock::Start(){
		start = std::clock();
	}

	void Clock::Stop(){
		duration = (double)(std::clock() - start) / CLOCKS_PER_SEC;
	}

	void Clock::Reset(){
		start = 0;
		duration = 0;
	}

	long Clock::GetTime(){
		std::time_t result = std::time(nullptr);
    return result;
	}

} // namespace internal
} // namespace Cobra
