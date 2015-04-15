#include "function.h"

namespace Cobra {
	Function::Function(std::string functionName, std::string functionArgs[Limits::FUNCTION_ARGUMENTS_LIMIT]){
		for (int i = 0; i < Limits::FUNCTION_ARGUMENTS_LIMIT; i++){
			args[i] = functionArgs[i];
		}
		name = functionName;

		printf("%s%s\n", "Creating function: ", name.c_str());
	}

	Function::~Function(){
		delete block;
	}

	void Function::setBlock(FunctionBlock* functionblock){
		block = functionblock;
	}
}