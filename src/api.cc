#include "api.h"

#include <fstream>
#include <streambuf>
#include <stdlib.h>

#include "cobra/flags.h"
#include "cobra/global.h"
#include "cobra/mem/isolate.h"
#include "cobra/ast/context.h"
#include "cobra/mem/allocate.h"
#include "cobra/types/strings/string.h"
#include "cobra/types/script/script.h"

namespace Cobra{

	const char* Version(){
		std::string result = "";
		result += std::to_string(MAJOR);
		result += ".";
		result += std::to_string(MINOR);
		result += ".";
		result += std::to_string(BUILD);
		return result.c_str();
	}

	Isolate* Isolate::New(){
		i::MemoryPool* mp = new i::MemoryPool(sizeof(i::Isolate), sizeof(i::Isolate), sizeof(i::Isolate));
		void* p = mp->GetMemory(sizeof(i::Isolate));
		i::Isolate* isolate = new(p) i::Isolate();
		isolate->SetSelfPool(mp);
		return CAST(Isolate*, isolate);
	}

	void Isolate::Enter(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Enter();
	}

	void Isolate::Exit(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Exit();
	}

	void Isolate::Dispose(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Dispose();
	}

	Context* Isolate::CreateContext(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		i::Context* context = isolate->CreateContext();
		return CAST(Context*, context);
	}

	void SetCommandLineFlags(int argc, const char* argv[]){
		i::Flags::SetCommandLineFlags(argc, argv);
	}

	void Context::Dispose(){
		i::Context* context = CAST(i::Context*, this);
		i::Isolate* isolate = context->GetIsolate();
		isolate->FreeMemory(context, sizeof(Context));
	}

	String* String::NewFromFile(Isolate* isolate, const char* path){
		//char* absolutePath = realpath(path, NULL); error on windows
		const char* absolutePath = path;
		std::ifstream in(absolutePath);
		if (!in){
			printf("File, %s, was empty\n", absolutePath);
			return String::New(isolate);
		}
		std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		String* h = String::New(isolate, fileStr.c_str(), absolutePath);
		delete absolutePath;
		return h;
	}

	String* String::New(Isolate* isolate, const char* string){
		return String::New(isolate, string, "inline");
	}

	String* String::New(Isolate* isolate){
		return String::New(isolate, "", "inline");
	}

	String* String::New(Isolate* isolate, const char* string, const char* path){
		i::Isolate* iso = CAST(i::Isolate*, isolate);
		i::String* str = i::String::New(iso);
		str->SetPath(path);
		str->SetValue(string);
		return CAST(String*, str);
	}

	Script* Script::Compile(Isolate* isolate, String* string){
		i::Isolate* iso = CAST(i::Isolate*, isolate);
		i::String* str = CAST(i::String*, string);
		i::Script* script = i::Script::New(iso, str);
		script->Compile();
		return CAST(Script*, script);
	}

	void Script::Run(){
		i::Script* script = CAST(i::Script*, this);
		script->Run();
	}
}