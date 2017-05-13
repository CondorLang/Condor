// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <vector>
#include <map>
#include <string>

#include "condor/mem/isolate.h"
#include "condor/ast/node.h"

namespace Condor {
namespace internal{

	class Isolate;
	class Scope;
	class ASTNode;
	
	class Context
	{
	private:
		Isolate* isolate;
		std::vector<std::string> inProgress;
		std::vector<Scope*> root;
		std::map<int, Scope*> registry;
		std::map<std::string, bool> included;
		bool allExport;
		int nextAstId;

	public:
		Context();
		~Context();
		inline void Enter(){}
		inline void Exit(){}
		void SetIsolate(Isolate* isolate);
		Isolate* GetIsolate(){return this->isolate;}
		void AllExport(){allExport = true;}
		void AddToInProgress(std::string str);
		void AddToRegistry(Scope* scope);
		Scope* GetFromRegistry(int scopeId);

		int GetNextAstId();
		void AddScope(Scope* scope);
		bool IsIncluded(std::string name);
		std::vector<ASTNode*> Lookup(Scope* scope, std::string name);
		std::vector<Scope*> injectedScopes;
	};

} // namespace internal
}

#endif // CONTEXT_H_
