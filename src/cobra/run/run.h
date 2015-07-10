#ifndef RUN_H_
#define RUN_H_

#include <string>
#include "../token/token.h"
#include "../ast/scope.h"
#include "../ast/ast.h"
#include "../error/error.h"
#include "../parser/parser.h"
#include "../ast/check.h"

namespace Cobra {

	class Run
	{
	public:
		Run();
		~Run();
		Parser* parser;
		Check* check;

		void Execute();
		void Compile(std::string* source);
		void Dispose();

	private:
		std::map<std::string, ASTFile*> files;
	};
}

#endif // RUN_H_
