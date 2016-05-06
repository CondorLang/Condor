// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <Cobra.h>
#include <string>

using namespace Cobra;

int main(int argc, const char* argv[]){
	SetCommandLineFlags(argc, argv);
	Isolate* isolate = Isolate::New();
	Context* context = isolate->CreateContext();

	context->Enter();
	String* string = String::NewFromBase(isolate);
	Script* script = Script::Compile(context, string);
	script->Run();

	context->Exit();
	context->Dispose();
	isolate->Dispose();
}