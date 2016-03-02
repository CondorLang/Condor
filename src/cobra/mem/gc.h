#ifndef GC_H_
#define GC_H_

#include "cobra/ast/ast.h"
#include "allocate.h"

namespace Cobra {
namespace internal{

	class GC
	{
	public:
		GC();
		~GC(){}
		void Run(MemoryPool* pool);
	};

} // namespace internal
} // namespace Cobra

#endif // GC_H_