// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef TYPES_CLOCK_H_
#define TYPES_CLOCK_H_

#include <vector>
#include "condor/mem/isolate.h"

namespace Condor {
namespace internal{

	class Clock;

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
		int AddClock();
		Clock* GetClock(unsigned int i){if (Clocks.size() <= i) return NULL; return Clocks[i];}
	};

} // namespace internal
}

#endif // TYPES_CLOCK_H_
