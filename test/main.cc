#include <stdio.h>
#include <Cobra.h>
#include <string>

using namespace Cobra;

int main(int argc, const char* argv[]){
	SetCommandLineFlags(argc, argv);
	Context* context = Context::New();
	Isolate* isolate = Isolate::New();
	context->SetIsolate(isolate);

	isolate->Enter();
	Handle* handle = String::NewFromFile(isolate, "test/test.cb");
	Handle* script = Script::Compile(isolate, handle);
	script->Run();

	isolate->Exit();
	isolate->Dispose();
}