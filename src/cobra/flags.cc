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
			else if (arg == "-h"){
				printf("\n\nCobraLang\n=========\n(C) 2015 Cobra Authors\n\n");
				printf("--trace-parser\t\t%s\n", "Trace the path of the parser");
				printf("--trace-check\t\t%s\n", "Trace the path of the ast checker");
				printf("--print-variables\t%s\n", "Prints the variables inside each scope");
				printf("--parsing-time\t\t%s\n", "Prints the time it took to parse");
			}
		}
	}

	bool Flags::traceParser = false;
	bool Flags::traceCheck = false;
	bool Flags::printVariables = false;
	bool Flags::parsingTime = false;

} // namespace internal
} // namespace Cobra