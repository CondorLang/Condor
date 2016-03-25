#include "clock.h"

namespace Cobra {
namespace internal{

	void Clock::Start(){
		start = std::clock();
	}

	void Clock::Stop(){
		duration = (double)(std::clock() - start) / CLOCKS_PER_SEC;
	}
	void Clock::Reset(){
		start = 0;
		duration = 0;
	}

} // namespace internal
} // namespace Cobra
