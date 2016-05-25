// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <Condor.h>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <vector>

using namespace Condor;

int main(int argc, const char* argv[]){
	Condor::Initialize();
	SetCommandLineFlags(argc, argv);
	Isolate* isolate = Isolate::New();
	Context* context = isolate->CreateContext();

	context->Enter();
	std::vector<std::string> tests = Directory::ReadDir("test/tests", true);
	for (int i = 0; i < tests.size(); i++){
		if (!Directory::IsDir(tests[i])){
			String* string = String::NewFromFile(isolate, tests[i].c_str());
			Script* script = Script::Compile(context, string);
			script->Run();
			if (script->HasError()){
				printf("Test Failed: %s\n", tests[i].c_str());
				exit(-1);
			}
			script->Free(isolate);
			string->Free(isolate);
		}
	}

	context->Exit();
	context->Dispose();
	isolate->Dispose();
}