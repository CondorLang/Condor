#include "vector.h"
#include "cobra/assert.h"
#include "cobra/mem/isolate.h"
#include "cobra/mem/allocate.h"

namespace Cobra {
namespace internal{

	void* NewVectorItem(Isolate* isolate, size_t size){
		kCount++;
		if (isolate == NULL) P("Missing Isolate in vector");
		void* v = isolate->GetMemory(size);
		if (v == NULL) OutOfMemory();
		return v;
	}

	void OutOfMemory(){
		Allocate::OutOfMemory();
	}

	void InvalidVectorAccess(int idx, int size){
		printf("Invalid Vector access at index: %d (size = %d)\n", idx, size);
	}

	void InternalVectorError(int idx, int size){
		printf("Internal Vector error at index: %d (size = %d)\n", idx, size);
	}

	void EraseVectorItem(Isolate* isolate, size_t size, void* ptr){
		isolate->FreeMemory(ptr, size);
	}

	void P(const char* msg){
		printf("%s\n", msg);
	}

	void P(int i){
		printf("%d\n", i);
	}

} // namespace internal
}