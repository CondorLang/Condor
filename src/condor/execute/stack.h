// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef STACK_H_
#define STACK_H_

#include <string>
#include <vector>

#include "condor/mem/isolate.h"
#include "condor/ast/scope.h"
#include "condor/ast/node.h"

namespace Condor {
namespace internal{

	class RPNStack
	{
	public:
		static RPNStack* New(Isolate* isolate);
		std::vector<ASTNode*> opStack;
		std::vector<ASTNode*> stack;
		
	};

} // namespace internal
} // namespace Condor

#endif // STACK_H_