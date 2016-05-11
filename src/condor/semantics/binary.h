// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef BINARY_H_
#define BINARY_H_

#include "condor/error/error.h"
#include "condor/token/token.h"

namespace Condor {
namespace internal{

	class Binary
	{
	public:
		static TOKEN Compare(TOKEN leftT, TOKEN right, TOKEN op);
	};

} // namespace internal
}

#endif // BINARY_H_
