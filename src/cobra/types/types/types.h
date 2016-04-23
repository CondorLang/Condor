#ifndef TYPES_H_
#define TYPES_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "types-bytes.h"

namespace Cobra {
namespace internal{

	class Types
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
	};

} // namespace internal
}

#endif // TYPES_H_
