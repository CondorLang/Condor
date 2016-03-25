#ifndef CLOCK_H_
#define CLOCK_H_

#include <ctime>

namespace Cobra {
namespace internal{

	class Clock
	{
	private:
		double duration;
		std::clock_t start;
	public:
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