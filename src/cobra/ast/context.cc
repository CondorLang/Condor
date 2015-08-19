#include "context.h"

namespace Cobra {
namespace internal{
	
	Context::Context(){
		isolate = NULL;
	}

	Context::~Context(){

	}

	void Context::SetIsolate(Isolate* isolate){
		this->isolate = isolate;
		isolate->SetContext(this);
	}

} // namespace internal
}