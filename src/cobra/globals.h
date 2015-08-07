#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

// see https://github.com/v8/v8/blob/master/src/globals.h

namespace Cobra {
namespace internal{

	#define BASE_ISOLATE_LOCATION 0x12345678
	
	typedef uint8_t byte; // since byte doesn't exist
	typedef byte* Address;


} // namespace internal
} // namespace Cobra

namespace i = Cobra::internal;

#endif // GLOBAL_H_