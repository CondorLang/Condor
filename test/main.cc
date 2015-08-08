#include <stdio.h>
#include <Cobra.h>
#include <fstream>
#include <streambuf>
#include <string>

using namespace Cobra;

// #include "tests/simple.h"

// void runFile();
// void runTests1();

int main(){
	Isolate* isolate = Isolate::New();
	isolate->Enter();
	
	Handle* handle = String::New(isolate, "test");
	if (handle->IsString()){
		printf("String found: %s\n", handle->ToString());
	}

	isolate->Exit();
	isolate->Dispose();
	//runFile();
	//runTests1();
}

// void runFile(){
// 	std::ifstream in("test/test.cb");
// 	std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
// 	Compile(&fileStr);
// }

// void runTests1(){
// 	for (int i = 0; i < goodSimpleSize; i++){
// 		Compile(&goodSimple[i]);
// 		printf("Test %d: Complete\n", i + 1);
// 	}
// }