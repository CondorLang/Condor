#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <map>
#include "ast.h"
#include "cobra/globals.h"
#include "cobra/types/vector/vector.h"

namespace Cobra {
namespace internal{
	class ASTNode;
	class Type;

	class Scope
	{
	private:
		Vector<ASTNode*> ordered;

		int count;
	public:
		Scope();
		~Scope();
		static Scope* New(Isolate* iso);
		Scope* outer;

		Scope* NewScope(Isolate* isolate);
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
