// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GC_H_
#define GC_H_

#include <vector>
#include <map>

#include "allocate.h"
#include "isolate.h"
#include "condor/ast/scope.h"
#include "condor/token/token.h"

namespace Condor {
namespace internal{

	class Scope;
	class Isolate;

	class GC
	{
	private: 
		std::vector<Scope*> scopes;
		std::map<int, bool> cleansed;
		bool printIds;
		Scope* GetCurrentScope(){if (scopes.size() == 0) return NULL; return scopes[0];}
		void OpenScope(Scope* scope){scopes.insert(scopes.begin(), scope);}
		void CloseScope(){scopes.erase(scopes.begin());}
		bool InParentScopes(int id);
	public:
		GC();
		~GC(){}
		static GC* New(Isolate* isolate);
		void Run(MemoryPool* pool);
		void Clear();
		void Dispose(Isolate* isolate, Scope* scope, bool deep, bool objKeys = false);
		void Dispose(Isolate* isolate, ASTNode* node, bool deep, bool objKeys = false);
	};

} // namespace internal
} // namespace Condor

#endif // GC_H_