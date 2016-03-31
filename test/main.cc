#include <stdio.h>
#include <Cobra.h>
#include <string>

using namespace Cobra;

int main(int argc, const char* argv[]){
	SetCommandLineFlags(argc, argv);
	Isolate* isolate = Isolate::New();
	Context* context = isolate->CreateContext();

	context->Enter();
	String* string = String::NewFromFile(isolate, "test/test.cb");
	Script* script = Script::Compile(context, string);
	script->Run();

	context->Exit();
	context->Dispose();
	isolate->Dispose();
}