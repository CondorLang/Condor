// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "stack.h"

namespace Cobra {
namespace internal{

	RPNStack* RPNStack::New(Isolate* isolate){
		void* pt = isolate->GetMemory(sizeof(RPNStack));
		RPNStack* n = new(pt) RPNStack();
		return n;
	}

} // namespace internal
} // namespace Cobra