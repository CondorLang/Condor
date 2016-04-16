#ifndef GC_H_
#define GC_H_

#include "allocate.h"
#include "isolate.h"
#include "cobra/ast/scope.h"

namespace Cobra {
namespace internal{

	class Scope;
	class Isolate;

	class GC
	{
	public:
		GC();
		~GC(){}
		void Run(MemoryPool* pool);
		void Dispose(Isolate* isolate, Scope* scope);
		void Dispose(Isolate* isolate, ASTNode* node);
	};

} // namespace internal
} // namespace Cobra

#endif // GC_H_