#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <map>
#include "ast.h"
#include "cobra/globals.h"

namespace Cobra {
namespace internal{
	class ASTNode;
	class Type;

	class Scope
	{
	private:
		std::vector<ASTNode*> ordered;

		int count;
	public:
		Scope();
		~Scope();
		Scope* outer;

		Scope* NewScope();
		std::vector<ASTNode*> Lookup(std::string name);
		void Insert(ASTNode* node);
		void String();
		ASTNode* Get(int index);
		int Size();
		void InsertBefore(ASTNode* node);
	};
} // namespace internal
}

#endif // SCOPE_H_
