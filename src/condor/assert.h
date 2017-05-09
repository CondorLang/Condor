// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef ASSERT_H_
#define ASSERT_H_

#include <stdlib.h> 
#include "condor/mem/isolate.h" 
#include "condor/flags.h"

namespace Condor {
namespace internal{
	
	void FATAL(const char* file, int line, const char* msg1, const char* msg2);

	#define CHECK(condition) \
		if (!(condition)){ \
			Condor::internal::FATAL(__FILE__, __LINE__, "Fatal:", #condition); \
		}		

	#define BENCHMARK(node, name) \
		if (node != NULL){\
			printf("Function: %s, NodeId: %d\n", name, node->id);\
		}

	#define TRACK(node) \
		if (node != NULL && node->id == Flags::trackNode){ \
			printf("%d - Passing Through - %s\n", node->id, __func__); \
		}

} // namespace internal
} // namespace Condor

#endif // ASSERT_H_