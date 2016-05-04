// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "console.h"

namespace Cobra {
namespace internal{

	void Console::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ConsoleBytes, "console", sub);
	}

} // namespace internal
}