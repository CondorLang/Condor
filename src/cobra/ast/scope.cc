#include "scope.h"
#include "cobra/mem/isolate.h"

namespace Cobra{
namespace internal{
	Scope::Scope(){
		isParsed = false;
		outer = NULL;
		owner = NULL;
	}

	Scope::~Scope(){}

	Scope* Scope::New(Isolate* isolate){
		void* pt = isolate->GetMemory(sizeof(Scope));
		Scope* n = new(pt) Scope();
		n->isolate = isolate;
		return n;
	}

	void Scope::Insert(ASTNode* node){
		if (node == NULL) return;
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

	std::vector<ASTNode*> Scope::Lookup(std::string name, bool deep, bool exported){
		std::vector<ASTNode*> results;
		for (int i = 0; i < nodes.size(); i++){
			if (exported){
				if (nodes[i]->name == name && nodes[i]->isExport) results.push_back(nodes[i]);
			}
			else{
				if (nodes[i]->name == name) results.push_back(nodes[i]);
			}
		}
		if (results.size() == 0 && outer != NULL && deep) return outer->Lookup(name);
		else if (results.size() == 0 && outer == NULL && deep) return isolate->GetContext()->Lookup(name);
		return results;
	}

	void Scope::Merge(Scope* scope){
		if (scope == NULL) return;
		std::vector<ASTNode*> kNodes = scope->GetNodes();
		nodes.insert(nodes.end(), kNodes.begin(), kNodes.end());
	}

	void Scope::Destroy(ASTNode* node){
		for (int i = 0; i < nodes.size(); i++){
			if (nodes[i] == node) {
				isolate->FreeMemory(node, node->Size());
				nodes.erase(nodes.begin() + i);
			}
		}
	}

	void Scope::Destroy(){
		isolate->FreeMemory(this, sizeof(Scope));
	}

	void Scope::RemoveAllAfter(ASTNode* node){
		bool found = false;
		int idx = -1;
		for (int i = 0; i < nodes.size(); i++){
			if (nodes[i] == node) {
				found = true;
				idx = i + 1;
				break;
			}
		}
		if (idx != -1 && nodes.size() > idx){
			for (int i = nodes.size(); i >= idx; i--){
				ASTNode* node = nodes[i];
				isolate->FreeMemory(node, node->Size());
				nodes.erase(nodes.begin() + i);
			}
		}
	}
} // namespace internal
} // namespace Cobra	