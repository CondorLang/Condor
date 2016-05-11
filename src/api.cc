// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "api.h"

#include <fstream>
#include <streambuf>
#include <stdlib.h>
#include <ctime>
#include <locale>

#include "stone/flags.h"
#include "stone/global.h"
#include "stone/assert.h"
#include "stone/mem/isolate.h"
#include "stone/ast/context.h"
#include "stone/mem/allocate.h"
#include "stone/types/strings/string.h"
#include "stone/types/script/script.h"

namespace Stone{

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

	void Context::Enter(){
		i::Context* context = CAST(i::Context*, this);
		context->Enter();
	}

	void Context::Exit(){
		i::Context* context = CAST(i::Context*, this);
		context->Exit();
	}

	void Isolate::Dispose(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Dispose();
		isolate->~Isolate();
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

	String* String::NewFromBase(Isolate* isolate){
		if (i::Flags::baseFile.empty()) return String::New(isolate);
		return String::NewFromFile(isolate, i::Flags::baseFile.c_str());
	}

	String* String::NewFromFile(Isolate* isolate, const char* path){
		std::string pth(path);
		#ifdef _WIN32
			std::replace(pth.begin(), pth.end(), '/', '\\');
		#endif
		std::ifstream in(pth);
		if (!in){
			printf("File, %s, was empty\n", pth.c_str());
			return String::New(isolate);
		}
		std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		String* h = String::New(isolate, fileStr.c_str(), pth.c_str());
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
		if (str->GetPathStr() == "inline"){ // unique name hack
			std::locale::global(std::locale(""));
	    std::time_t t = std::time(NULL);
	    char mbstr[100];
	    if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t))) {
	        str->name = mbstr;
	    }
		}
		return CAST(String*, str);
	}

	Script* Script::Compile(Context* context, String* string){
		i::Context* con = CAST(i::Context*, context);
		i::String* str = CAST(i::String*, string);
		i::Script* script = i::Script::New(con, str);
		script->Compile();
		return CAST(Script*, script);
	}

	void Script::Run(){
		i::Script* script = CAST(i::Script*, this);
		script->Run();
	}

	bool Script::HasError(){
		i::Script* script = CAST(i::Script*, this);
		return script->HasError();
	}

	std::string Script::GetErrorMsg(){
		i::Script* script = CAST(i::Script*, this);
		return script->GetErrorMsg();
	}

	bool String::IsEmpty(){
		i::String* str = CAST(i::String*, this);
		return str->IsEmpty();
	}
}