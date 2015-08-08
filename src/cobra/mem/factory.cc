#include "factory.h"

namespace Cobra {
namespace internal{

	Factory::~Factory(){}

	String* Factory::NewString(const char* str){
		String* string = new String();
		string->SetValue(str);
		return string;
	}

	Script* Factory::NewScript(Handle* handle){
		Script* script = new Script(handle);
		return script;
	}

} // namespace internal
} // namespace Cobra