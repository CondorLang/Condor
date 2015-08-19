#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "cobra/parser/parser.h"
#include "cobra/ast/check.h"
#include "cobra/mem/handle.h"
#include "cobra/ast/ast.h"
#include "cobra/assert.h"
#include <vector>
#include <map>
#include <fstream>
#include <streambuf>

namespace Cobra {
namespace internal{

	class Handle;
	class Parser;
	class Check;

	class Script
	{
	private:
		Handle* source;
		Parser* parser;
		Check* check;
		bool hasErr;
		std::vector<const char*> msgs;
		const char* basePath;
		std::map<std::size_t, ASTFile*> includes;
		void SetIncludes(std::vector<ASTInclude*> includes);
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
