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
		std::vector<ASTNode*> ordered;
		int count;
	public:
		Scope();
		~Scope();
		Scope* outer;

		Scope* NewScope();
		ASTNode* Lookup(std::string name);
		void Insert(ASTNode* node);
		ASTNode* NewObject(std::string name);
		void String();
		ASTNode* Get(int index);
		int Size();
	};
}

#endif // SCOPE_H_
