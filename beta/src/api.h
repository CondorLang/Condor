#ifndef API_H_
#define API_H_

#include "../include/Cobra.h"

namespace Cobra {

	namespace internal{
		class Isolate;
	}

	#define MAJOR 0
	#define MINOR 1
	#define BUILD 0

} // namespace Cobra

namespace i = Cobra::internal;

#endif // API_H_