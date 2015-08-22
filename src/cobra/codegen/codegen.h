#ifndef CODEGEN_H_
#define CODEGEN_H_

// Reference: http://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
// 		http://www.zynamics.com/binnavi.html

#include "cobra/ast/context.h"

namespace Cobra {
namespace internal{
	
	class CodeGen
	{
	private:
		Context* baseContext;
	public:
		CodeGen(Context* context);
		~CodeGen();		
	};

} // namespace internal
} // namespace Cobra

#endif // CODEGEN_H_