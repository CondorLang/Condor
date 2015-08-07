#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::~Isolate(){

	}

	void Isolate::Enter(){

	}

	void Isolate::Exit(){
		
	}

	HeapObject* Isolate::Insert(HeapObject obj){
		return heapstore.Insert(obj);
	}

} // namespace internal
} // namespace Cobra