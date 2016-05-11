// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef API_H_
#define API_H_

#include "../include/Stone.h"

namespace Stone {

	namespace internal{
		class Isolate;
	}

	#define MAJOR 0
	#define MINOR 6
	#define BUILD 0

} // namespace Stone

namespace i = Stone::internal;

#endif // API_H_