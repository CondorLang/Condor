// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef STACK_H_
#define STACK_H_

#include <string>
#include <vector>

#include "cobra/mem/isolate.h"
#include "cobra/ast/scope.h"
#include "cobra/ast/node.h"

namespace Cobra {
namespace internal{

	class RPNStack
	{
	public:
		static RPNStack* New(Isolate* isolate);
		std::vector<ASTNode*> opStack;
		std::vector<ASTNode*> stack;
		
	};

} // namespace internal
} // namespace Cobra

#endif // STACK_H_