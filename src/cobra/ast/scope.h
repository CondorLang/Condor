#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <map>
#include "ast.h"

namespace Cobra {
	class ASTNode;

	class Scope
	{
	private:
		std::map<std::string, ASTNode*> objects;
	public:
		Scope();
		~Scope();
		Scope* outer;

		Scope* NewScope();
		ASTNode* Lookup(std::string name);
		void Insert(ASTNode* node);
		ASTNode* NewObject(std::string name);
	};
}

#endif // SCOPE_H_
