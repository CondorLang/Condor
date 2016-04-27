#ifndef APP_H_
#define APP_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "app-bytes.h"

namespace Cobra {
namespace internal{

	class App
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
		static bool Included;
	};

} // namespace internal
}

#endif // APP_H_
