#include "context.h"

namespace Cobra {
namespace internal{
	
	Context::Context(){
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