#include "context.h"
#include "scope.h"

namespace Cobra {
namespace internal{
	
	Context::Context(){
		isolate = NULL;
	}

	Context::~Context(){
		
	}

	void Context::SetIsolate(Isolate* isolate){
		this->isolate = isolate;
	}

	void Context::AddToInProgress(std::string str){
		inProgress.push_back(str);
	}

	int Context::GetNextAstId(){
		return nextAstId++;
	}

	bool Context::IsIncluded(std::string name){
		return included.find(name) != included.end();
	}

	void Context::AddScope(Scope* scope){
		scope->context = this;
		root.push_back(scope);
		included[scope->name] = true;
	}

	std::vector<ASTNode*> Context::Lookup(std::string name){
		std::vector<ASTNode*> result;
		for (int i = 0; i < root.size(); i++){
			std::vector<ASTNode*> tmp = root[i]->Lookup(name, false, true);
			result.insert(result.end(), tmp.begin(), tmp.end());
		}
		return result;
	}

} // namespace internal
}