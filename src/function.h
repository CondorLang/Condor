#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <string>
#include <stdio.h>
#include <map>
#include "limits.h"
#include "var.h"

namespace Cobra {

	class FunctionBlock
	{
	private:
		std::map<std::string, Var> variables;
		
	public:
		FunctionBlock();
		~FunctionBlock(){}
	};

	class Function
	{
	private:
		std::string name;
		std::string args[Limits::FUNCTION_ARGUMENTS_LIMIT];
		FunctionBlock* block;

	public:
		Function(std::string functionName, std::string functionArgs[Limits::FUNCTION_ARGUMENTS_LIMIT]);
		~Function();
		void setBlock(FunctionBlock* functionblock);
	};
}

#endif // FUNCTION_H_	
