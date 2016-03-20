#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <map>
#include <vector>

#include "cobra/global.h"
#include "cobra/mem/isolate.h"
#include "cobra/ast/node.h"

namespace Cobra {
namespace internal{
	class ASTNode;
	class Type;

	class Scope
	{
	private:
		bool isParsed;
		std::string* raw;
		std::vector<ASTNode*> nodes;
		
	public:
		Scope();
		~Scope();
		static Scope* New(Isolate* isolate);
		void Insert(ASTNode* node);
		int Size();
		ASTNode* Get(int idx);
	};
} // namespace internal
}

#endif // SCOPE_H_
