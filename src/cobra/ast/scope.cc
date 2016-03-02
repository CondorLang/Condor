#include "scope.h"
#include "cobra/mem/isolate.h"

namespace Cobra{
namespace internal{
	Scope::Scope(){
		count = 1;
		outer = NULL;
	}

	Scope::~Scope(){}

	Scope* Scope::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(Scope));
		Scope* s = new(p) Scope();
		s->count = 1;
		s->outer = NULL;
		return s;
	}

	Scope* Scope::NewScope(Isolate* isolate){
		Scope* scope = Scope::New(isolate);
		scope->outer = this;
		return scope;
	}

	std::vector<ASTNode*> Scope::Lookup(std::string name){
		std::vector<ASTNode*> objs;
		for (int i = 0; i < ordered.size(); i++){
			if (ordered[i]->name == name && ordered[i]->type != FUNC_CALL) objs.push_back(ordered[i]);
		}
		return objs;
	}

	void Scope::Insert(ASTNode* node){
		if (node != NULL){
			ordered.push_back(node);
			count++;
		}
	}

	ASTNode* Scope::Get(int index){
		return ordered[index];
	}

	int Scope::Size(){
		return ordered.size();
	}

	void Scope::String(){
		for (int i = 0; i < ordered.size(); i++){
			printf("Found: %s\n", ordered[i]->name.c_str());
		}
	}

	void Scope::InsertBefore(ASTNode* node){
		if (node != NULL){
			ordered.insert(ordered.begin(), node);
			count++;
		}
	}
} // namespace internal
} // namespace Cobra	