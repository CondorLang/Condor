// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "console-bytes.h"

namespace Cobra {
namespace internal{

	class Console
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
	};

} // namespace internal
}

#endif // CONSOLE_H_
