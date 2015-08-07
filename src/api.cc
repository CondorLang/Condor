#include "../include/Cobra.h"
#include "api.h"

namespace Cobra{

	Isolate::Isolate(){}
	Isolate::~Isolate(){}

	void* Isolate::operator new(size_t size){
		return Isolate::New();
	}

	void Isolate::Enter(){
		i::Isolate* isolate = reinterpret_cast<i::Isolate*>(this);
		isolate->Enter();
	}

	void Isolate::Exit(){
		i::Isolate* isolate = reinterpret_cast<i::Isolate*>(this);
		isolate->Enter();
	}

	Isolate* Isolate::New(){
		i::Isolate* isolate = new i::Isolate();
		return reinterpret_cast<Isolate*>(isolate);
	}

}