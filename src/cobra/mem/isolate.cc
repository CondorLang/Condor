#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::~Isolate(){

	}

	void Isolate::_enter(){
		_currentIsolate = this;
	}

	void Isolate::_exit(){
		_currentIsolate = NULL;
	}

	HeapObject* Isolate::Insert(HeapObject obj){
		return heapstore->Insert(obj);
	}

} // namespace internal
} // namespace Cobra