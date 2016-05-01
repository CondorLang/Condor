#ifndef DATE_H_
#define DATE_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "date-bytes.h"

namespace Cobra {
namespace internal{

	class Date
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
	};

} // namespace internal
}

#endif // DATE_H_
