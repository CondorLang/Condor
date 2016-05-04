// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef TYPES_CLOCK_H_
#define TYPES_CLOCK_H_

#include <vector>

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "clock-bytes.h"
#include "cobra/clock.h"

namespace Cobra {
namespace internal{

	class CBClock // clashing names
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
	};

	class CBClockContainer
	{
	private:
		Isolate* isolate;
	public:
		static CBClockContainer* New(Isolate* iso);
		CBClockContainer(Isolate* iso){isolate = iso;}
		~CBClockContainer(){}
		static CBClockContainer* GlobalClockContainer;
		std::vector<Clock*> Clocks;
		int AddClock(){Clocks.push_back(Clock::New(isolate)); return (int)Clocks.size();}
		Clock* GetClock(int i){if (Clocks.size() <= i) return NULL; return Clocks[i];}
	};

} // namespace internal
}

#endif // TYPES_CLOCK_H_
