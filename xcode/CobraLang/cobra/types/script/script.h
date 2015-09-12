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
#include "cobra/shell.h"
#include <string>

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
		bool internal;

		Handle* source;
		iHandle<Parser>* parser;
		iHandle<Check>* check;
		bool hasErr;
		std::vector<std::string> msgs;
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
		std::string GetSourceRow(int row, int col);
		int CharsToNewLine(std::string code, int start);

	public:
		Script(Handle* handle, Isolate* isolate);
		~Script(){}
		void Compile();
		Handle* GetSource(){return source;}
		std::string GetSourceCode();
		bool HasError(){return hasErr;}
		Isolate* GetIsolate(){return isolate;}
		const char* GetAbsolutePath(){return absolutePath.c_str();}
		std::string GetFileName();
		void Run();
		ASTNode* GetExportedObject(std::string name);
		const char* GetErrorMsg(){if (msgs.size() > 0) return msgs[0].c_str(); else return "";}
		static void RunInternalScript(Isolate* isolate, std::string hex);
	};

} // namespace internal
}

#endif // SCRIPT_H_
