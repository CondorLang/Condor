#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "cobra/parser/parser.h"
#include "cobra/ast/check.h"
#include "cobra/mem/handle.h"
#include "cobra/ast/ast.h"
#include "cobra/assert.h"
#include "cobra/ast/context.h"
#include "cobra/types/vector/vector.h"
#include "cobra/types/script/script.h"
#include "cobra/types/strings/string.h"
#include <map>
#include <fstream>
#include <streambuf>
#include <stdlib.h>
#include "cobra/clock.h"
#include "cobra/shell.h"
#include <string>
#include <cstring>

namespace Cobra {
namespace internal{

	class Handle;
	class Parser;
	class Check;
	template<class T> class Pointer;

	class Script
	{
	private:

		//flags
		bool parsingTime;
		bool internal;

		Handle* source;
		Parser* parser;
		Check* check;
		bool hasErr;
		Vector<std::string> msgs;
		std::string name;
		const char* basePath;
		std::map<std::size_t, ASTFile*> includes;
		void SetIncludes();
		void SetImports();
		std::string absolutePath;
		std::string sourceCode;
		Isolate* isolate;
		bool compiled;
		std::string GetPathOfImport(std::string import);
		std::string ParseRelativePath(std::string absolute, std::string path);
		std::string BackFolder(std::string path);
		std::string StayInFolder(std::string path);		
		std::string GetSourceRow(int row, int col);
		int CharsToNewLine(std::string code, int start);

	public:
		Script(){}
		static Script* New(Handle* string, Isolate* isolate);
		void Compile();
		void SetDefualts(Handle* string, Isolate* isolate);
		Handle* GetSource(){return source;}
		std::string GetSourceCode();
		bool HasError(){return hasErr;}
		Isolate* GetIsolate(){return isolate;}
		const char* GetAbsolutePath(){return absolutePath.c_str();}
		std::string GetFileName();
		void Run();
		ASTNode* GetExportedObject(std::string name);
		const char* GetErrorMsg(){if (msgs.size() > 0) return msgs[0].c_str(); else return "";}
		static void RunInternalScript(Isolate* isolate, std::string hex, std::string _name);
		void PrintExported();
	};

	namespace Sizes{
		const int kScript = sizeof(Script);
	}

} // namespace internal
}

#endif // SCRIPT_H_
