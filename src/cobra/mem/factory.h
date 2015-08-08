#ifndef FACTORY_H_
#define FACTORY_H_

#include "cobra/types/string.h"
#include "cobra/types/script.h"

namespace Cobra {
namespace internal{

	class String;
	class Script;
	class Handle;

	class Factory
		{
		public:
			Factory(){}
			~Factory();
			
			String* NewString(const char* str);
			Script* NewScript(Handle* handle);
		};	

} // namespace internal
} // namespace Cobra

#endif // FACTORY_H_