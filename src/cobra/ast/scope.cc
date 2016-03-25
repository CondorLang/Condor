#include "scope.h"
#include "cobra/mem/isolate.h"

namespace Cobra{
namespace internal{
	Scope::Scope(){
		isParsed = false;
		outer = NULL;
	}

	Scope::~Scope(){}

	Scope* Scope::New(Isolate* isolate){
		void* pt = isolate->GetMemory(sizeof(Scope));
		Scope* n = new(pt) Scope();
		return n;
	}

	void Scope::Insert(ASTNode* node){
		nodes.push_back(node);
	}

	int Scope::Size(){
		return nodes.size();
	}

	ASTNode* Scope::Get(int idx){
		return nodes.at(idx);
	}

	void Scope::InsertBefore(ASTNode* node){
		nodes.insert(nodes.begin(), node);
	}

	std::vector<ASTNode*> Scope::Lookup(std::string name, bool deep){
		std::vector<ASTNode*> results;
		for (int i = 0; i < nodes.size(); i++){
			if (nodes[i]->name == name) results.push_back(nodes[i]);
		}
		if (results.size() == 0 && outer != NULL && deep) return outer->Lookup(name);
		return results;
	}
} // namespace internal
} // namespace Cobra	