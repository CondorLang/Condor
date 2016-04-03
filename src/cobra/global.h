#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <time.h>
#include <string>

namespace Cobra {
namespace internal{

	#define CAST(T, V) reinterpret_cast<T>(V);
	#define BUILD_YEAR_CH0 (__DATE__[ 7])
	#define BUILD_YEAR_CH1 (__DATE__[ 8])
	#define BUILD_YEAR_CH2 (__DATE__[ 9])
	#define BUILD_YEAR_CH3 (__DATE__[10])
	#define HERE() printf("d: %s\n", "here");
	
	typedef uint8_t byte; // since byte doesn't exist
	typedef byte* Address;


} // namespace internal
} // namespace Cobra

#endif // GLOBAL_H_