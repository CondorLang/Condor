#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "cobra/types/script/script.h"
#include "console-bytes.h"

namespace Cobra {
namespace internal{

	class Console
	{
	public:
		Console(){}
		~Console(){}
		static void CB(Isolate* isolate);
	};

} // namespace internal
}

#endif // CONSOLE_H_
