#include "scope.h"
#include "cobra/mem/isolate.h"

namespace Cobra{
namespace internal{
	Scope::Scope(){
		isParsed = false;
		raw = NULL;
	}

	Scope::~Scope(){}
} // namespace internal
} // namespace Cobra	