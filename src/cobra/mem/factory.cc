#include "factory.h"

namespace Cobra {
namespace internal{

	Factory::~Factory(){}

	String* Factory::NewString(const char* str){
		String* string = new String(isolate); // Do not insert into heap, this is controlled in the API
		string->SetValue(str);
		return string;
	}

	Script* Factory::NewScript(Handle* handle){
		Script* script = new Script(handle, isolate); // Do not insert into heap, this is controlled in the API
		return script;
	}

} // namespace internal
} // namespace Cobra