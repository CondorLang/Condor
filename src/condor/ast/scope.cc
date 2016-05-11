// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "scope.h"
#include "condor/mem/isolate.h"

namespace Condor{
namespace internal{
	Scope::Scope(){
		isParsed = false;
		outer = NULL;
		owner = NULL;
		shallow = false;
	}

	Scope::~Scope(){}

	Scope* Scope::New(Isolate* isolate){
		void* pt = isolate->GetMemory(sizeof(Scope));
		Scope* n = new(pt) Scope();
		n->isolate = isolate;
		n->scopeId = scopeIdInc++;
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
			if (exported && nodes[i]->name == name && nodes[i]->isExport) results.push_back(nodes[i]);
			else if (!exported && nodes[i]->name == name) results.push_back(nodes[i]);
		}		
		if (outer != NULL && deep) {
			std::vector<ASTNode*> kNodes = outer->Lookup(name);
			results.insert(results.end(), kNodes.begin(), kNodes.end());
		}
		else if (outer == NULL && deep) {
			std::vector<ASTNode*> kNodes = isolate->GetContext()->Lookup(this, name);
			results.insert(results.end(), kNodes.begin(), kNodes.end());
		}
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
				node->Free(isolate);
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
				node->Free(isolate);
				if (nodes.size() == i){
					nodes.erase(nodes.end() - 1);
				}
				else{
					nodes.erase(nodes.begin() + i);
				}
			}
		}
	}

	void Scope::Remove(ASTNode* node){
		for (int i = 0; i < nodes.size(); i++){
			if (nodes[i] == node){
				nodes.erase(nodes.begin() + i);
				break;
			}
		}
	}
} // namespace internal
} // namespace Condor	