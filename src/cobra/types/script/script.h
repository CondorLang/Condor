#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "cobra/parser/parser.h"
#include "cobra/ast/check.h"
#include "cobra/mem/handle.h"
#include "cobra/ast/ast.h"
#include "cobra/assert.h"
#include "cobra/ast/context.h"
#include <vector>
#include <map>
#include <fstream>
#include <streambuf>
#include <stdlib.h>

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
		std::string absolutePath;
		Isolate* isolate;
		bool compiled;
		const char* GetPathOfImport(std::string import);
		bool Replace(std::string& str, const std::string& from, const std::string& to);

	public:
		Script(Handle* handle, Isolate* isolate);
		~Script(){}
		void Compile();
		Handle* GetSource(){return source;}
		bool HasError(){return hasErr;}
		Isolate* GetIsolate(){return isolate;}
		const char* GetAbsolutePath(){return absolutePath.c_str();}
		std::string GetFileName();
	};

} // namespace internal
}

#endif // SCRIPT_H_
