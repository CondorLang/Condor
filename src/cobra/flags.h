#ifndef FLAGS_H_
#define FLAGS_H_

#include <stdio.h>
#include <string>

namespace Cobra {
namespace internal{

	#define TRACE_PARSER Flags::traceParser
	#define TRACE_CHECK Flags::traceCheck
	#define PRINT_VARIABLES Flags::printVariables;
	#define PARSING_TIME Flags::parsingTime;

	class Flags
	 {
	 public:
	 		static bool traceParser;
	 		static bool traceCheck;
	 		static bool printVariables;
	 		static bool parsingTime;
	 		static void SetCommandLineFlags(int argc, const char* argv[]);
	 }; 

} // namespace internal
} // namespace Cobra

#endif // FLAGS_H_