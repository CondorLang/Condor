#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "cobra/parser/parser.h"
#include "cobra/ast/check.h"
#include "cobra/mem/handle.h"
#include "cobra/ast/ast.h"
#include "cobra/assert.h"
#include <vector>

namespace Cobra {
namespace internal{

	class Handle;

	class Script
	{
	private:
		Handle* source;
		Parser* parser;
		Check* check;
		bool hasErr;
		std::vector<const char*> msgs;
	public:
		Script(Handle* handle);
		~Script(){}
		void Compile();
		Handle* GetSource(){return source;}
		bool HasError(){return hasErr;}
	};

} // namespace internal
}

#endif // SCRIPT_H_
