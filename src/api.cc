#include "api.h"

#include <fstream>
#include <streambuf>

#include "cobra/mem/isolate.h"
#include "cobra/types/string.h"
#include "cobra/mem/handle.h"
#include "cobra/token/token.h"
#include "cobra/globals.h"
#include "cobra/types/script.h"

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
		return String::New(isolate, "", "inline");
	}

	Handle* String::New(Isolate* isolate, const char* string){
		return String::New(isolate, string, "inline");
	}

	Handle* String::New(Isolate* isolate, const char* string, const char* path){
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		i::String* str = cobra_isolate->factory->NewString(string);
		str->SetPath(path);
		i::HeapObject obj;
		obj.address = CAST(i::Address, str);
		obj.type = i::STRING;
		i::HeapObject* newStr = cobra_isolate->Insert(obj);
		i::Handle* handle = new i::Handle(newStr);
		return CAST(Handle*, handle);
	}

	Handle* String::NewFromFile(Isolate* isolate, const char* path){
		std::ifstream in(path);
		if (!in) return String::New(isolate);
		std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return String::New(isolate, fileStr.c_str(), path);
	}

	bool Handle::IsString(){
		i::Handle* handle = CAST(i::Handle*, this);
		return handle->GetObject()->type == i::STRING;
	}

	bool Handle::IsScript(){
		i::Handle* handle = CAST(i::Handle*, this);
		return handle->GetObject()->type == i::SCRIPT;
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

	Handle* Script::Compile(Isolate* isolate, Handle* handle){
		i::Handle* cobra_handle = CAST(i::Handle*, handle);
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		i::Script* script = cobra_isolate->factory->NewScript(cobra_handle);

		script->Compile();

		i::HeapObject obj;
		obj.address = CAST(i::Address, script);
		obj.type = i::SCRIPT;
		i::HeapObject* newScript = cobra_isolate->Insert(obj);
		i::Handle* scriptHandle = new i::Handle(newScript);
		return CAST(Handle*, scriptHandle);
	}

}