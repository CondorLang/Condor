// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef ISOLATE_H_
#define ISOLATE_H_

#include "allocate.h"
#include "condor/ast/context.h"
#include "condor/ast/scope.h"
#include "condor/ast/node.h"

namespace Condor {
namespace internal{

	class Context;
	class GC;

	class Isolate
	{
	private:
		MemoryPool* medium;
		MemoryPool* large;
		MemoryPool* small;
		MemoryPool* xl;
		MemoryPool* self;
		GC* gc;
		Context* context;
		bool printGcIds;
		int gcPassId;

	public:
		Isolate();
		~Isolate();
		static Isolate* GLOBAL_ISOLATE;
		static Isolate* CurrentIsolate;
		/**
		 * Used to dispose memory that hasn't been released upon force kill
		 */
		static void ForceDispose(int signum);
		Context* CreateContext();
		Context* GetContext(){return context;}
		void* GetMemory(const size_t size);
		template<class T>
		void FreeMemory(T* ptr, const size_t size){
			ptr->~T();
			if (size < 35) small->FreeMemory(ptr, size);
			else if (size < 80) medium->FreeMemory(ptr, size);
			else if (size < 130) large->FreeMemory(ptr, size);
			else xl->FreeMemory(ptr, size);
		}
		void SetSelfPool(MemoryPool* mp){this->self = mp;}
		void Dispose();
		void RunGC(Scope* scope, bool deep = false, bool objKeys = false);
		void RunGC(ASTNode* node, bool deep = false, bool objKeys = false);
		int MemoryAudit();
	};

} // namespace internal
} // namespace Condor

#endif // ISOLATE_H_