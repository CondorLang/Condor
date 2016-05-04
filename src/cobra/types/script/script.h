// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <string>
#include <map>
#include <vector>

#include "cobra/flags.h"
#include "cobra/clock.h"
#include "cobra/shell.h"
#include "cobra/ast/context.h"
#include "cobra/mem/isolate.h"	
#include "cobra/error/error.h"
#include "cobra/parser/parser.h"
#include "cobra/semantics/semantics.h"
#include "cobra/execute/execute.h"

#include "cobra/types/strings/string.h"	
#include "cobra/types/array/array.h"
#include "cobra/types/console/console.h"
#include "cobra/types/try_catch/exception.h"
#include "cobra/types/types/types.h"
#include "cobra/types/path/path.h"
#include "cobra/types/fs/fs.h"
#include "cobra/types/app/app.h"
#include "cobra/types/clock/clock.h"
#include "cobra/types/date/date.h"
#include "cobra/types/integer/integer.h"

namespace Cobra {
namespace internal{

	class String;
	class Parser;
	class Semantics;
	class Execute;

	class Script
	{
	private:
		String* raw;

		//flags
		bool parsingTime;
		bool compileTime;
		bool executionTime;
		bool internal;

		Parser* parser;
		String* source;
		std::string* currentCode;
		Semantics* semantics;
		Execute* executor;
		bool hasErr;
		std::vector<std::string> msgs;
		std::string name;
		const char* basePath;
		std::string absolutePath;
		std::string sourceCode;
		Isolate* isolate;
		Context* context;
		bool compiled;
		std::string subModule;

		std::string GetSourceRow(int row, int col);
		void LoadImports();
		void LoadIncludes();

	public:
		Script(Isolate* isolate, String* str, Context* con);
		~Script(){}
		static Script* New(Context* context, String* str);
		static void RunInternalScript(Isolate* isolate, std::string hex, std::string _name, std::string sub);
		void Run();
		bool HasError(){return hasErr;}
		std::string GetErrorMsg(){if (msgs.size() > 0) return msgs[0]; return "";}
		void Compile();
		void SetSub(std::string sub){subModule = sub;}
	};

	namespace Sizes{
		const int kScript = sizeof(Script);
	}

} // namespace internal
}

#endif // SCRIPT_H_
