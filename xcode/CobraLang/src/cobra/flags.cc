#include "flags.h"

namespace Cobra {
namespace internal{

	void Flags::SetCommandLineFlags(int argc, const char* argv[]){
		for (int i = 0; i < argc; i++){
			std::string arg = argv[i];
			if (arg == "--trace-parser") Flags::traceParser = true;
			else if (arg == "--trace-check") Flags::traceCheck = true;
			else if (arg == "--print-variables") Flags::printVariables = true;
			else if (arg == "--parsing-time") Flags::parsingTime = true;
			else if (arg == "--heap-insert") Flags::heapInsert = true;
			else if (arg == "--heap-delete") Flags::heapDelete = true;
			else if (arg == "-h"){
				printf("\n\nCobraLang\n=========\n(C) 2015 Cobra Authors\n\n");
				printf("--trace-parser\t\t%s\n", "Trace the path of the parser");
				printf("--trace-check\t\t%s\n", "Trace the path of the ast checker");
				printf("--print-variables\t%s\n", "Prints the variables inside each scope");
				printf("--parsing-time\t\t%s\n", "Prints the time it took to parse");
				printf("--heap-insert\t\t%s\n", "Tracks what is being inserted into the heap");
				printf("--heap-delete\t\t%s\n", "Tracks what is being deleted out of the heap");
			}
			else if (arg.find("-") != std::string::npos || arg.find("-") != std::string::npos){
				printf("Unidentified flag\nExiting...\n");
				exit(0);
			}
		}
	}

	bool Flags::traceParser = false;
	bool Flags::traceCheck = false;
	bool Flags::printVariables = false;
	bool Flags::parsingTime = false;
	bool Flags::heapInsert = false;
	bool Flags::heapDelete = false;

} // namespace internal
} // namespace Cobra