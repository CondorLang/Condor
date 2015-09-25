#include "vector.h"
#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	void* NewVectorItem(Isolate* isolate, size_t size){
		return isolate->GetMemory(size);
	}

} // namespace internal
}