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
#include "cobra/clock.h"

namespace Cobra {
namespace internal{

	class Handle;
	class Parser;
	class Check;

	class Script
	{
	private:

		//flags
		bool parsingTime;

		Handle* source;
		Parser* parser;
		Check* check;
		bool hasErr;
		std::vector<const char*> msgs;
		const char* basePath;
		std::map<std::size_t, ASTFile*> includes;
		void SetIncludes();
		std::string absolutePath;
		Isolate* isolate;
		bool compiled;
		std::string GetPathOfImport(std::string import);
		std::string ParseRelativePath(std::string absolute, std::string path);
		std::string BackFolder(std::string path);
		std::string StayInFolder(std::string path);		

	public:
		Script(Handle* handle, Isolate* isolate);
		~Script(){}
		void Compile();
		Handle* GetSource(){return source;}
		bool HasError(){return hasErr;}
		Isolate* GetIsolate(){return isolate;}
		const char* GetAbsolutePath(){return absolutePath.c_str();}
		std::string GetFileName();
		void Run();
		ASTNode* GetExportedObject(std::string name);
	};

} // namespace internal
}

#endif // SCRIPT_H_
