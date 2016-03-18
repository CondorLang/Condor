#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <string>
#include <map>
#include <vector>

#include "cobra/flags.h"
#include "cobra/clock.h"
#include "cobra/shell.h"
#include "cobra/mem/isolate.h"	
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"
#include "cobra/types/strings/string.h"	

namespace Cobra {
namespace internal{

	class String;
	class Parser;

	class Script
	{
	private:
		String* raw;

		//flags
		bool parsingTime;
		bool internal;

		Parser* parser;
		String* source;
		bool hasErr;
		std::vector<std::string> msgs;
		std::string name;
		const char* basePath;
		std::string absolutePath;
		std::string sourceCode;
		Isolate* isolate;
		bool compiled;

		std::string GetSourceRow(int row, int col);

	public:
		Script(Isolate* isolate, String* str);
		~Script(){}
		static Script* New(Isolate* iso, String* str);
		static void RunInternalScript(Isolate* isolate, std::string hex, std::string _name);
		void Run();
		void Compile();
	};

	namespace Sizes{
		const int kScript = sizeof(Script);
	}

} // namespace internal
}

#endif // SCRIPT_H_
