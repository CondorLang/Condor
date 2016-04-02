#ifndef INTERNAL_H_
#define INTERNAL_H_

#include "cobra/ast/node.h"
#include "cobra/token/token.h"
#include "cobra/global.h"

#include <string>

namespace Cobra {
namespace internal{

	class ASTNode;
	typedef void (*InternalFunctionCallback)(ASTNode* node);

	class Internal
	{
	public:
		static void PrintF(ASTNode* lit);
		static void CallInternal(Isolate* iso, InternalFunctionCallback call, ASTNode* node){call(node);}
	};

	#define CALL_INTERNAL(iso, func, node) \
			Internal::CallInternal(iso, func, node);

} // namespace internal
}

#endif // INTERNAL_H_
