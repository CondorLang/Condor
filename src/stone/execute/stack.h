// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef STACK_H_
#define STACK_H_

#include <string>
#include <vector>

#include "stone/mem/isolate.h"
#include "stone/ast/scope.h"
#include "stone/ast/node.h"

namespace Stone {
namespace internal{

	class RPNStack
	{
	public:
		static RPNStack* New(Isolate* isolate);
		std::vector<ASTNode*> opStack;
		std::vector<ASTNode*> stack;
		
	};

} // namespace internal
} // namespace Stone

#endif // STACK_H_