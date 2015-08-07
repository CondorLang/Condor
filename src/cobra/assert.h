#ifndef ASSERT_H_
#define ASSERT_H_


namespace Cobra {
namespace internal{

	// TODO:
	// 		dispose and abort of Cobra
	void FETAL(const char* msg){
		printf("%s\n", msg);
	}


	#define CHECK(condition) \
		if (!condition){ \
			FETAL("Fetal: %s", #condition); \
		}

} // namespace internal
} // namespace Cobra

#endif // ASSERT_H_