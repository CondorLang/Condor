#include "flags.h"

namespace Cobra {
namespace internal{

	void Flags::SetCommandLineFlags(int argc, const char* argv[]){
		for (int i = 0; i < argc; i++){
			std::string arg = argv[i];
			if (arg == "--trace-parser") Flags::traceParser = true;
			else if (arg == "--trace-semantic") Flags::traceSemantic = true;
			else if (arg == "--print-variables") Flags::printVariables = true;
			else if (arg == "--print-exported") Flags::exportedNodes = true;
			else if (arg == "--parsing-time") Flags::parsingTime = true;
			else if (arg == "--memory-audit") Flags::memoryAudit = true;
			else if (arg == "--memory-request") Flags::memoryRequest = true;
			else if (arg == "--expand-ast") Flags::expandAST = true;
			else if (arg == "--allow-native") Flags::allowNative = true;
			else if (arg == "--compile-time") Flags::compileTime = true;
			else if (arg == "--trace-evaluation") Flags::traceEvaluation = true;
			else if (arg == "--execution-time") Flags::executionTime = true;
			else if (arg == "--execution-steps") Flags::evaluationSteps = true;
			else if (arg == "-h" || arg == "--help"){
				printf("\n\nCobraLang\n=========\n(C) 2015 Cobra Authors\n\n");
				printf("--trace-parser\t\t%s\n", "Trace the path of the parser");
				printf("--trace-semantic\t%s\n", "Trace the path of the ast semantic checker");
				printf("--trace-evaluation\t%s\n", "Trace the evaluation of the code");
				printf("--print-variables\t%s\n", "Prints the variables inside each scope");
				printf("--print-exported\t%s\n", "Prints the exported nodes in the context");
				printf("--parsing-time\t\t%s\n", "Prints the time it took to parse");
				printf("--compile-time\t\t%s\n", "Prints the time it took to compile");
				printf("--execution-time\t%s\n", "Prints the time it took to execute");
				printf("--memory-audit\t\t%s\n", "Audits the memory used and whats free");
				printf("--memory-request\t%s\n", "Tracks the memory requested");
				printf("--expand-ast\t\t%s\n", "Expand the abstract syntax tree");
				printf("--allow-native\t\t%s\n", "Allow native functions in code");
				printf("--execution-steps\t%s\n", "Traces every execution step");
			}
			else if (arg.find("-") != std::string::npos || arg.find("-") != std::string::npos){
				printf("Unidentified flag\nExiting...\n");
				exit(0);
			}
		}
	}

	bool Flags::traceParser = false;
	bool Flags::traceSemantic = false;
	bool Flags::printVariables = false;
	bool Flags::parsingTime = false;
	bool Flags::exportedNodes = false;
	bool Flags::memoryAudit = false;
	bool Flags::memoryRequest = false;
	bool Flags::expandAST = false;
	bool Flags::allowNative = false;
	bool Flags::compileTime = false;
	bool Flags::traceEvaluation = false;
	bool Flags::executionTime = false;
	bool Flags::evaluationSteps = false;

} // namespace internal
} // namespace Cobra