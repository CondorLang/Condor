#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	class Isolate;
	
	class Context
	{
	private:
		Isolate* isolate;
	public:
		Context();
		~Context();
		void SetIsolate(Isolate* isolate);
	};

} // namespace internal
}

#endif // CONTEXT_H_
