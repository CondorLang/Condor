#ifndef FACTORY_H_
#define FACTORY_H_

#include "cobra/types/string.h"

namespace Cobra {
namespace internal{

	class String;

	class Factory
		{
		public:
			Factory(){}
			~Factory();
			
			String* NewString(const char* str);
		};	

} // namespace internal
} // namespace Cobra

#endif // FACTORY_H_