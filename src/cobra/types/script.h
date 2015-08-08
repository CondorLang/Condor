#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "cobra/parser/parser.h"
#include "cobra/ast/check.h"
#include "cobra/mem/handle.h"
#include "cobra/ast/ast.h"

namespace Cobra {
namespace internal{

	class Handle;

	class Script
	{
	private:
		Handle* source;
		Parser* parser;
		Check* check;
	public:
		Script(Handle* handle) : source(handle){parser=NULL;check=NULL;}
		~Script(){}
		void Compile();
		Handle* GetSource(){return source;}
	};

} // namespace internal
}

#endif // SCRIPT_H_
