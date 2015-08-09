#ifndef ASSERT_H_
#define ASSERT_H_


namespace Cobra {
namespace internal{
	
	#define FETAL(msg1, msg2) \
		printf("%s %s\n", #msg1, #msg2);

	#define CHECK(condition) \
		if (!(condition)){ \
			FETAL("Fetal: %s", #condition); \
		}		

} // namespace internal
} // namespace Cobra

#endif // ASSERT_H_