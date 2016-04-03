#include <stdio.h>
#include <Cobra.h>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>

using namespace Cobra;

int main(int argc, const char* argv[]){
	SetCommandLineFlags(argc, argv);
	Isolate* isolate = Isolate::New();
	Context* context = isolate->CreateContext();

	context->Enter();
	std::string input = "";
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	int year = aTime->tm_year + 1900;
	std::cout << "CobraLang (C) " + std::to_string(year) + "\n";

	while (true){
		std::cout << ">> ";
		getline(std::cin, input);
		String* string = String::New(isolate, input.c_str());
		Script* script = Script::Compile(context, string);
		if (!script->HasError()) script->Run();
	}

	context->Exit();
	isolate->Dispose();
}