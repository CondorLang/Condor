#ifndef ARRAY_H_
#define ARRAY_H_

#include "cobra/types/script/script.h"
#include "array-bytes.h"

namespace Cobra {
namespace internal{

	class Array
	{
	public:
		Array(){}
		~Array(){}
		static void CB(Isolate* isolate, std::string sub);
	};

} // namespace internal
}

#endif // ARRAY_H_
