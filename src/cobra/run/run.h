#ifndef RUN_H_
#define RUN_H_

#include <string>
#include "cobra/token/token.h"
#include "cobra/ast/scope.h"
#include "cobra/ast/ast.h"
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"
#include "cobra/ast/check.h"
#include "cobra/types/type.h"
#include "cobra/types/func.h"

namespace Cobra {
namespace internal{

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
		void Init();
	};
} // namespace internal{
}

#endif // RUN_H_
