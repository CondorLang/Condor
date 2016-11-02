// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <map>
#include <vector>

#include "condor/global.h"
#include "condor/mem/isolate.h"
#include "condor/ast/node.h"
#include "condor/token/token.h"

namespace Condor {
namespace internal{
	class ASTNode;
	class Type;
	class Context;

	class Scope
	{
	private:
		bool isParsed;
		bool shallow;
		std::vector<ASTNode*> nodes;
		Isolate* isolate;
		
	public:
		Scope();
		~Scope();
		static int scopeIdInc;
		Scope* outer;
		std::string raw;
		std::string name;
		Context* context;
		int scopeId;
		std::vector<ASTNode*> local;
		ASTNode* owner;
		static Scope* New(Isolate* isolate);
		void Insert(ASTNode* node);
		void InsertBefore(ASTNode* node);
		int Size();
		bool IsShallow(){return shallow;}
		bool IsParsed(){return isParsed;}
		void SetShallow(){shallow = true;}
		void SetParsed(){isParsed = true;}
		void UnsetShallow(){shallow = false;}
		void Merge(Scope* scope, bool exportedOnly = true);
		void Destroy(ASTNode* node);
		void Destroy();
		void RemoveAllAfter(ASTNode* node);
		void Remove(ASTNode* node);
		std::vector<ASTNode*> GetNodes(){return nodes;}
		ASTNode* Get(int idx);
		std::vector<ASTNode*> Lookup(std::string name, bool deep = true, bool exported = false);
	};
} // namespace internal
}

#endif // SCOPE_H_
