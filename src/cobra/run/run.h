#ifndef RUN_H_
#define RUN_H_

#include <string>
#include "../token/token.h"
#include "../ast/scope.h"
#include "../ast/ast.h"
#include "../error/error.h"
#include "../parser/parser.h"

namespace Cobra {
	class Run
	{
	public:
		Run(std::string source);
		~Run();
		Parser* parser;

		void Start();

	private:
		std::map<std::string, ASTFile*> files;
		bool HasFunction(std::string name);
		bool HasObjectInScope(std::string name, Scope* scope);
		ASTNode* GetObject(std::string name);
		void Init();
		void CallFunction(ASTFunc* func);
	};
}

#endif // RUN_H_
