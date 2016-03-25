#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

// see https://github.com/v8/v8/blob/master/src/globals.h

namespace Cobra {
namespace internal{

	class Isolate;

	#define BASE_ISOLATE_LOCATION 0x12345678
	#define VERSION 0.1
	#define CAST(T, V) reinterpret_cast<T>(V);
	
	typedef uint8_t byte; // since byte doesn't exist
	typedef byte* Address;

	static Isolate* _currentIsolate = NULL;

} // namespace internal
} // namespace Cobra

#endif // GLOBAL_H_