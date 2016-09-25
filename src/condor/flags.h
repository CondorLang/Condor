// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef FLAGS_H_
#define FLAGS_H_

#include <stdio.h>
#include <string>
#include <cstdlib>
#include "condor/global.h"

namespace Condor {
namespace internal{

	#define TRACE_PARSER Flags::traceParser
	#define TRACE_SEMANTIC Flags::traceSemantic
	#define TRACE_EVALUATION Flags::traceEvaluation;
	#define PRINT_VARIABLES Flags::printVariables;
	#define PARSING_TIME Flags::parsingTime;
	#define EXPORTED_NODES Flags::exportedNodes;
	#define MAX_MEMORY_SIZE Flags::maxMemorySize;
	#define MEMORY_AUDIT Flags::memoryAudit;
	#define MEMORY_REQUEST Flags::memoryRequest;
	#define EXPAND_AST Flags::expandAST;
	#define ALLOW_NATIVE Flags::allowNative;
	#define COMPILE_TIME Flags::compileTime;
	#define EXECUTION_TIME Flags::executionTime;
	#define EVALUATION_STEPS Flags::evaluationSteps;
	#define RPN_STACK Flags::rpnStack;
	#define BASE_FILE Flags::baseFile;
	#define PRINT_GC_IDS Flags::printGCIds;

	class Flags
	 {
	 public:
	 		static bool traceParser;
	 		static bool traceSemantic;
	 		static bool printVariables;
	 		static bool parsingTime;
	 		static bool exportedNodes;
	 		static bool memoryAudit;
	 		static bool memoryRequest;
	 		static bool expandAST;
	 		static bool allowNative;
	 		static bool compileTime;
	 		static bool traceEvaluation;
	 		static bool executionTime;
	 		static bool evaluationSteps;
	 		static bool rpnStack;
	 		static std::string baseFile;
	 		static bool printGCIds;
	 		static void SetCommandLineFlags(int argc, const char* argv[]);
	 }; 

} // namespace internal
} // namespace Condor

#endif // FLAGS_H_