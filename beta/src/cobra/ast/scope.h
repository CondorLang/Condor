#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <map>
#include "cobra/global.h"
#include <vector>

namespace Cobra {
namespace internal{
	class ASTNode;
	class Type;

	class Scope
	{
	private:
		bool isParsed;
		std::string* raw;
		
	public:
		Scope();
		~Scope();
	};
} // namespace internal
}

#endif // SCOPE_H_
