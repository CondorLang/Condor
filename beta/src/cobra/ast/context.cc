#include "context.h"
#include "scope.h"

namespace Cobra {
namespace internal{
	
	Context::Context(){
		isolate = NULL;
		root = NULL;
	}

	Context::~Context(){
		
	}

	void Context::AddToInProgress(std::string str){
		inProgress.push_back(str);
	}

	int Context::GetNextAstId(){
		return nextAstId++;
	}

} // namespace internal
}