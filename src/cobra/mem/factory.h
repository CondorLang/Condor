#ifndef FACTORY_H_
#define FACTORY_H_

#include "cobra/types/strings/string.h"
#include "cobra/types/script/script.h"
#include "cobra/ast/ast.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class String;
	class Script;
	class Handle;

	class Factory
		{
		private:
			Isolate* isolate;
		public:
			Factory(Isolate* iso) : isolate(iso){}
			~Factory();
			
			String* NewString(const char* str);
			Script* NewScript(Handle* handle);
		};	

} // namespace internal
} // namespace Cobra

#endif // FACTORY_H_