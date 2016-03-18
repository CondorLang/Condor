#include <stdio.h>
#include <Cobra.h>
#include <string>

using namespace Cobra;

int main(int argc, const char* argv[]){
	SetCommandLineFlags(argc, argv);
	Isolate* isolate = Isolate::New();
	Context* context = isolate->CreateContext();

	isolate->Enter();
	String* string = String::NewFromFile(isolate, "test/test.cb");
	Script* script = Script::Compile(isolate, string);
	script->Run();

	isolate->Exit();
	context->Dispose();
	isolate->Dispose();
}