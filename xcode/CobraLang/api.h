#ifndef API_H_
#define API_H_

#include "../include/Cobra.h"

namespace Cobra {

	namespace internal{
		class Isolate;
	}

	#define API_VERSION 1.0

} // namespace Cobra

namespace i = Cobra::internal;

#endif // API_H_