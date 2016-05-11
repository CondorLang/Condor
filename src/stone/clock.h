// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef CLOCK_H_
#define CLOCK_H_

#include <ctime>
#include <map>
#include <ctime>

#include "stone/mem/isolate.h"

namespace Stone {
namespace internal{

	class Clock
	{
	private:
		double duration;
		std::clock_t start;
	public:
		static Clock* New(Isolate* isolate);
		Clock(){}
		~Clock(){}
		void Start();
		void Stop();
		void Reset();
		double GetDuration(){return duration;}
		static long GetTime();
		static std::string GetDate(double epoch, std::string format);
	};

} // namespace internal
} // namespace Stone

#endif // CLOCK_H_