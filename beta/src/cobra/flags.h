#ifndef FLAGS_H_
#define FLAGS_H_

#include <stdio.h>
#include <string>
#include <cstdlib>

namespace Cobra {
namespace internal{

	#define TRACE_PARSER Flags::traceParser
	#define TRACE_SEMANTIC Flags::traceSemantic
	#define PRINT_VARIABLES Flags::printVariables;
	#define PARSING_TIME Flags::parsingTime;
	#define EXPORTED_NODES Flags::exportedNodes;
	#define MAX_MEMORY_SIZE Flags::maxMemorySize;
	#define MEMORY_AUDIT Flags::memoryAudit;
	#define MEMORY_REQUEST Flags::memoryRequest;
	#define EXPAND_AST Flags::expandAST;
	#define ALLOW_NATIVE Flags::allowNative;
	#define PRINT_ASM Flags::printAsm;

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
	 		static bool printAsm;
	 		static void SetCommandLineFlags(int argc, const char* argv[]);
	 }; 

} // namespace internal
} // namespace Cobra

#endif // FLAGS_H_