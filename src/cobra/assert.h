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