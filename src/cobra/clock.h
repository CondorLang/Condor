#ifndef CLOCK_H_
#define CLOCK_H_

#include <ctime>
#include <map>

#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	class Clock
	{
	private:
		double duration;
		std::clock_t start;
	public:
		static Clock* New(Isolate* isolate);
		Clock(){}
		~Clock(){}
		void Start();
		void Stop();
		void Reset();
		double GetDuration(){return duration;}
	};

} // namespace internal
} // namespace Cobra

#endif // CLOCK_H_