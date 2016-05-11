// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "clock.h"

namespace Condor {
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

	std::string Clock::GetDate(double epoch, std::string format){
		std::time_t ti = static_cast<std::time_t>(epoch);
		std::time_t t = std::time(&ti);
		struct tm *tm = localtime(&t);
		char date[180];
		strftime(date, sizeof(date), format.c_str(), tm);
		return date;
	}

} // namespace internal
} // namespace Condor
