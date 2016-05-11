// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <string>
#include <map>
#include <vector>

#include "stone/flags.h"
#include "stone/clock.h"
#include "stone/shell.h"
#include "stone/ast/context.h"
#include "stone/mem/isolate.h"	
#include "stone/error/error.h"
#include "stone/parser/parser.h"
#include "stone/semantics/semantics.h"
#include "stone/execute/execute.h"

#include "stone/types/strings/string.h"	
#include "stone/types/path/path.h"
#include "stone/types/fs/fs.h"
#include "stone/types/app/app.h"
#include "stone/types/clock/clock.h"

namespace Stone {
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
		static void RunInternalScript(Isolate* isolate, std::string hex, std::string _name, std::string sub, bool isHex = true);
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
