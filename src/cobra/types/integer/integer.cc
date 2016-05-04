// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "integer.h"

namespace Cobra {
namespace internal{

	void Integer::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, IntegerBytes, "integer", sub);
	}

} // namespace internal
}