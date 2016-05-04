// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "array.h"

namespace Cobra {
namespace internal{

	void Array::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ArrayBytes, "array", sub);
	}

} // namespace internal
}