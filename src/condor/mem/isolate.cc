// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "isolate.h"
#include "gc.h"

namespace Condor {
namespace internal{

	Isolate::Isolate(){
		const static size_t poolSize = KB * 8; // 8kb
		const static size_t chunkSize2 = 35;
		const static size_t sizeToAllocate2 = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		small = new MemoryPool(poolSize, chunkSize2, sizeToAllocate2);
		small->name = "small";

		const static size_t chunkSize = 80;
		const static size_t sizeToAllocate = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		medium = new MemoryPool(poolSize, chunkSize, sizeToAllocate);
		medium->name = "medium";

		const static size_t chunkSize3 = 130;
		const static size_t sizeToAllocate3 = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		large = new MemoryPool(poolSize, chunkSize3, sizeToAllocate3);
		large->name = "large";

		xl = new MemoryPool();
		xl->name = "xl";

		bool requested = MEMORY_REQUEST;
		if (requested){
			small->debug = true;
			medium->debug = true;
			large->debug = true;
			xl->debug = true;
		}
		gc = new GC();
		Isolate::CurrentIsolate = this;
	}

	Isolate::~Isolate(){
		delete small;
		delete medium;
		delete large;
		delete xl;
		delete gc;
		delete this->self;
	}

	Isolate* Isolate::GLOBAL_ISOLATE = NULL;

	void Isolate::ForceDispose(int signum){
		if (Isolate::GLOBAL_ISOLATE == NULL) return;
		printf("\n%s", "Force disposing...");
		Isolate* isolate = Isolate::GLOBAL_ISOLATE;
		Context* context = isolate->GetContext();
		Condor::Context* con = CAST(Condor::Context*, context);
		con->Dispose();
		Condor::Isolate* iso = CAST(Condor::Isolate*, isolate);
		iso->Dispose();
		printf("%s\n", "Done");
		exit(signum);
	}

	Isolate* Isolate::CurrentIsolate = NULL;

	void Isolate::Dispose(){
		this->~Isolate();
	}

	Context* Isolate::CreateContext(){
		void* p = this->GetMemory(sizeof(Context));
		Context* context = new(p) Context();
		context->SetIsolate(this);
		this->context = context;
		return context;
	}

	void* Isolate::GetMemory(const size_t size){
		if (size < 35){
			return small->GetMemory(size);
		}
		else if (size < 80){
			return medium->GetMemory(size);
		}
		else if (size < 130){
			return large->GetMemory(size);
		}
		else{
			return xl->GetMemory(size);
		}
	}

	// void Isolate::FreeMemory(void* ptr, const size_t size){
	// 	if (size < 35) small->FreeMemory(ptr, size);
	// 	else if (size < 80) medium->FreeMemory(ptr, size);
	// 	else if (size < 130) large->FreeMemory(ptr, size);
	// 	else xl->FreeMemory(ptr, size);
	// }

	void Isolate::RunGC(Scope* scope, bool deep, bool objKeys){
		gc->Dispose(this, scope, deep, objKeys);
	}

	void Isolate::RunGC(ASTNode* node, bool deep, bool objKeys){
		gc->Dispose(this, node, deep, objKeys);
	}

	int Isolate::MemoryAudit(){
		int total = small->GetMemoryUsage();
		total += medium->GetMemoryUsage();
		total += large->GetMemoryUsage();
		total += xl->GetMemoryUsage();
		return total;
	}

} // namespace internal
} // namespace Condor