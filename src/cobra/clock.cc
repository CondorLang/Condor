#include "clock.h"

namespace Cobra {
namespace internal{

	void Clock::Start(){
		start = std::clock();
	}

	void Clock::Stop(){
		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	}
	void Clock::Reset(){
		start = NULL;
	}

} // namespace internal
} // namespace Cobra
