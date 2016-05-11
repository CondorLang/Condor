// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef BINARY_H_
#define BINARY_H_

#include "stone/error/error.h"
#include "stone/token/token.h"

namespace Stone {
namespace internal{

	class Binary
	{
	public:
		static TOKEN Compare(TOKEN leftT, TOKEN right, TOKEN op);
	};

} // namespace internal
}

#endif // BINARY_H_
