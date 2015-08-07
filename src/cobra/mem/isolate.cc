#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::~Isolate(){

	}

	void Isolate::Enter(){
		printf("%s\n", "test");
	}

	void Isolate::Exit(){
		printf("%s\n", "test");
	}

	HeapObject* Isolate::Insert(HeapObject obj){
		return heapstore->Insert(obj);
	}

} // namespace internal
} // namespace Cobra