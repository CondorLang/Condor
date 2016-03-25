#ifndef BINARY_H_
#define BINARY_H_

#include "cobra/error/error.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class Binary
	{
	public:
		static TOKEN Compare(TOKEN leftT, TOKEN right, TOKEN op);
	};

} // namespace internal
}

#endif // BINARY_H_
