#include <stdio.h>
#include <Cobra.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace Cobra;

int main(int argc, const char* argv[]){
	SetCommandLineFlags(argc, argv);
	Context* context = Context::New();
	Isolate* isolate = Isolate::New();
	context->SetIsolate(isolate);

	isolate->Enter();
	std::string input = "";
	std::cout << "CobraLang (C) 2015\n";
	while (true){
		std::cout << ">> ";
		getline(std::cin, input);
		Handle* handle = String::New(isolate, input.c_str());
		Handle* script = Script::Compile(isolate, handle);
		script->Run();
	}

	isolate->Exit();
	isolate->Dispose();
}