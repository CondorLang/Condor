// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef ASSERT_H_
#define ASSERT_H_


namespace Cobra {
namespace internal{
	
	#define FETAL(msg1, msg2) \
		printf("%s %s\n", #msg1, #msg2); exit(1);

	#define CHECK(condition) \
		if (!(condition)){ \
			FETAL("Fetal:", #condition); \
		}		

	#define BENCHMARK(node, name) \
		if (node != NULL){\
			printf("Function: %s, NodeId: %d\n", name, node->id);\
		}

} // namespace internal
} // namespace Cobra

#endif // ASSERT_H_