#include "api.h"
#include "cobra/mem/isolate.h"
#include "cobra/types/string.h"
#include "cobra/mem/handle.h"
#include "cobra/token/token.h"
#include "cobra/globals.h"

namespace Cobra{

	void Isolate::Dispose(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		delete isolate;
	}

	void* Isolate::operator new(size_t size){
		return Isolate::New();
	}

	void Isolate::Enter(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Enter();
	}

	void Isolate::Exit(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Exit();
	}
	
	Isolate* Isolate::New(){
		i::Isolate* isolate = new i::Isolate();
		return CAST(Isolate*, isolate);
	}

	Isolate* Isolate::GetCurrent(){
		i::Isolate* isolate = i::_currentIsolate;
		if (isolate != NULL){
			return CAST(Isolate*, isolate);
		}
		return NULL;
	}

	Handle* String::New(Isolate* isolate){
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		i::String* str = cobra_isolate->factory->NewString(""); // empty string
		i::HeapObject obj;
		obj.address = CAST(i::Address, str);
		obj.type = i::STRING;
		i::HeapObject* newStr = cobra_isolate->Insert(obj);
		i::Handle* handle = new i::Handle(newStr);
		return CAST(Handle*, handle);
	}

	Handle* String::New(Isolate* isolate, const char* string){
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		i::String* str = cobra_isolate->factory->NewString(string);
		i::HeapObject obj;
		obj.address = CAST(i::Address, str);
		obj.type = i::STRING;
		i::HeapObject* newStr = cobra_isolate->Insert(obj);
		i::Handle* handle = new i::Handle(newStr);
		return CAST(Handle*, handle);
	}

	bool Handle::IsString(){
		i::Handle* handle = CAST(i::Handle*, this);
		return handle->GetObject()->type == i::STRING;
	}

	const char* Handle::ToString(){
		if (IsString()){
			i::Handle* handle = CAST(i::Handle*, this);
			i::HeapObject* obj = handle->GetObject();
			i::String* str = CAST(i::String*, obj->address);
			return str->GetValue();
		}
		return NULL;
	}

}