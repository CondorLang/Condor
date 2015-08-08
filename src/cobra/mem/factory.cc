#include "factory.h"

namespace Cobra {
namespace internal{

	Factory::~Factory(){}

	String* Factory::NewString(const char* str){
		String* string = new String();
		string->SetValue(str);
		return string;
	}

} // namespace internal
} // namespace Cobra