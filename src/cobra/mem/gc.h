#ifndef GC_H_
#define GC_H_

#include <vector>

#include "allocate.h"
#include "isolate.h"
#include "cobra/ast/scope.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class Scope;
	class Isolate;

	class GC
	{
	private: 
		std::vector<Scope*> scopes;
		Scope* GetCurrentScope(){if (scopes.size() == 0) return NULL; return scopes[0];}
		void OpenScope(Scope* scope){scopes.insert(scopes.begin(), scope);}
		void CloseScope(){scopes.erase(scopes.begin());}
		bool InParentScopes(int id);
	public:
		GC();
		~GC(){}
		static GC* New(Isolate* isolate);
		void Run(MemoryPool* pool);
		void Dispose(Isolate* isolate, Scope* scope, bool deep);
		void Dispose(Isolate* isolate, ASTNode* node, bool deep);
	};

} // namespace internal
} // namespace Cobra

#endif // GC_H_