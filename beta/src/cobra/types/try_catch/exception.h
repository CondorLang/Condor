#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>
#include "../include/Cobra.h"
#include "cobra/mem/isolate.h"
#include "cobra/types/script/script.h"
#include "exception-bytes.h"

namespace Cobra {
namespace internal{

	class Exception
	{
	public:
		Exception(){}
		~Exception(){}
		static void CB(Isolate* isolate);
	};

	namespace Sizes{
		const int kException = sizeof(Exception);
	}

} // namespace internal
}

#endif // EXCEPTION_H_
