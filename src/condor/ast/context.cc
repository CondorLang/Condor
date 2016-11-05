// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "context.h"
#include "scope.h"

namespace Condor {
namespace internal{
	
	Context::Context(){
		isolate = NULL;
		allExport = false;
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

	void Context::AddToRegistry(Scope* scope){
		registry[scope->scopeId] = scope;
	}

	Scope* Context::GetFromRegistry(int scopeId){
		if (registry.find(scopeId) == registry.end()) return NULL;
		return registry[scopeId];
	}

	void Context::AddScope(Scope* scope){
		scope->context = this;
		scope->AllExport(allExport);
		root.push_back(scope);
		included[scope->name] = true;
	}

	std::vector<ASTNode*> Context::Lookup(Scope* scope, std::string name){
		std::vector<ASTNode*> result;
		for (unsigned int i = 0; i < root.size(); i++){
			Scope* s = root[i];
			if (s == scope) continue;
			std::vector<ASTNode*> tmp = root[i]->Lookup(name, false, true);
			if (tmp.size() > 0) result.insert(result.end(), tmp.begin(), tmp.end());
		}
		return result;
	}

} // namespace internal
}