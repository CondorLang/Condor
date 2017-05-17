// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "scope.h"

namespace Condor{
namespace internal{

	Scope::Scope(){
		isParsed = false;
		outer = NULL;
		owner = NULL;
		shallow = false;
	}

	Scope::~Scope(){
		while (nodes.begin() != nodes.end()){
			nodes[0]->Free(isolate);
			nodes.erase(nodes.begin());
		}
		raw.erase();
	}

	int Scope::scopeIdInc = 1;

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
		return (int) nodes.size();
	}

	ASTNode* Scope::Get(int idx){
		return nodes.at((unsigned long long int) idx);
	}

	void Scope::InsertBefore(ASTNode* node){
		nodes.insert(nodes.begin(), node);
	}

	std::vector<ASTNode*> Scope::Lookup(std::string name, bool deep, bool exported){
		std::vector<ASTNode*> results;
		if (allExport) exported = false;
		for (unsigned int i = 0; i < nodes.size(); i++){
			if (exported && nodes[i]->name == name && nodes[i]->isExport) results.push_back(nodes[i]);
			else if (!exported && nodes[i]->name == name) results.push_back(nodes[i]);
		}		
		if (outer != NULL && deep) {
			std::vector<ASTNode*> kNodes = outer->Lookup(name);
			if (kNodes.size() > 0) results.insert(results.end(), kNodes.begin(), kNodes.end());
		}
		else if (outer == NULL && deep) {
			std::vector<ASTNode*> kNodes = isolate->GetContext()->Lookup(this, name);
			if (kNodes.size() > 0) results.insert(results.end(), kNodes.begin(), kNodes.end());
		}
		return results;
	}

	void Scope::Merge(Scope* scope, bool exportedOnly){
		if (scope == NULL) return;
		std::vector<ASTNode*> kNodes = scope->GetNodes();
		for (unsigned int i = 0; i < kNodes.size(); i++){
			kNodes[i]->importScopeId = scope->scopeId;
			if (exportedOnly && kNodes[i]->isExport) nodes.insert(nodes.begin(), kNodes[i]);
		}
		if (!exportedOnly) nodes.insert(nodes.begin(), kNodes.begin(), kNodes.end());
	}

	void Scope::Destroy(ASTNode* node){
		for (unsigned int i = 0; i < nodes.size(); i++){
			if (nodes[i] == node) {
				node->Free(isolate);
				nodes.erase(nodes.begin() + i);
			}
		}
	}

	std::string Scope::ToString(int tabs){
		std::string results;
		for (unsigned int i = 0; i < nodes.size(); i++){
			results += nodes[i]->ToString(tabs);
		}
		return results;
	}

	void Scope::Destroy(){
		//this->~Scope();
		isolate->FreeMemory(this, sizeof(Scope));
	}

	void Scope::Remove(ASTNode* node){
		for (unsigned int i = 0; i < nodes.size(); i++){
			if (nodes[i] == node){
				nodes.erase(nodes.begin() + i);
				break;
			}
		}
	}
} // namespace internal
} // namespace Condor	