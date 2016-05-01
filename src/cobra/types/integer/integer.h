#ifndef INTEGER_H_
#define INTEGER_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "integer-bytes.h"

namespace Cobra {
namespace internal{

	class Integer
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
	};

} // namespace internal
}

#endif // INTEGER_H_
