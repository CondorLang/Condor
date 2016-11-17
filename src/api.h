// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef API_H_
#define API_H_

#include "../include/Condor.h"

namespace Condor {

	namespace internal{
		class Isolate;
	}

	#define MAJOR 0
	#define MINOR 6
	#define BUG 0

} // namespace Condor

namespace i = Condor::internal;

#endif