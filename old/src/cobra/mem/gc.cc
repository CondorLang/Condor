#include "gc.h"

namespace Cobra {
namespace internal{

	GC::GC(){

	}

	void GC::Run(MemoryPool* pool){
		pool->FreeUnnecessaryMemory();
	}

} // namespace internal
} // namespace Cobra
