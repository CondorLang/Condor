// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <string>
#include <map>
#include <vector>

#include "platform/windows.h"

#include "condor/flags.h"
#include "condor/clock.h"
#include "condor/shell.h"
#include "condor/ast/context.h"
#include "condor/mem/isolate.h"	
#include "condor/error/error.h"
#include "condor/parser/parser.h"
#include "condor/semantics/semantics.h"
#include "condor/execute/execute.h"

#include "condor/types/strings/string.h"	
#include "condor/types/path/path.h"
#include "condor/types/fs/fs.h"
#include "condor/types/app/app.h"
#include "condor/types/clock/clock.h"

namespace Condor {
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
		void Free(Isolate* isolate);

	public:
		Script(Isolate* isolate, String* str, Context* con);
		~Script(){}
		static Script* New(Context* context, String* str);
		static void RunInternalScript(Isolate* isolate, std::string hex, std::string _name, std::string sub, bool isHex = true);
		void Run();
		bool HasError(){return hasErr;}
		std::string GetErrorMsg(){if (msgs.size() > 0) return msgs[0]; return "";}
		void Compile();
		void SetSub(std::string sub){subModule = sub;}
		void Free(Isolate* isolate);
	};

	namespace Sizes{
		const int kScript = sizeof(Script);
	}

} // namespace internal
}

#endif // SCRIPT_H_
