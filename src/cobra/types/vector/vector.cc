#include "vector.h"
#include "cobra/assert.h"
#include "cobra/mem/isolate.h"
#include "cobra/mem/allocate.h"

namespace Cobra {
namespace internal{

	void* NewVectorItem(Isolate* isolate, size_t size){
		kCount++;
		void* v = isolate->GetMemory(size);
		return v;
	}

	void OutOfMemory(){
		Allocate::OutOfMemory();
	}

} // namespace internal
}