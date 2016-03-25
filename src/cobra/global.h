#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

namespace Cobra {
namespace internal{

	#define CAST(T, V) reinterpret_cast<T>(V);
	
	typedef uint8_t byte; // since byte doesn't exist
	typedef byte* Address;

} // namespace internal
} // namespace Cobra

#endif // GLOBAL_H_