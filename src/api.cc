#include "../include/Cobra.h"
#include "api.h"

namespace Cobra{

	Isolate::Isolate(){}
	Isolate::~Isolate(){}
	Isolate* Isolate::New(){
		i::Isolate* isolate = reinterpret_cast<i::Isolate*>(BASE_ISOLATE_LOCATION);
		Isolate* Cobra_isolate = reinterpret_cast<Isolate*>(isolate);
		return Cobra_isolate;
	}

}