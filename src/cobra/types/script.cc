#include "script.h"
#include <stdio.h> // printf doesn't work without this

namespace Cobra {
namespace internal{

	// TODO:
	// 		Add TRY_CATCH
	// 		Pass Isolate through
	void Script::Compile(){
		std::string sourceCode = "";
		sourceCode += source->ToString();
		parser = new Parser(&sourceCode);
		check = new Check();
		ASTFile* file = NULL;

		try {
			file = parser->Parse();
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, parser->expected);
			printf("%d:%d - %s\n", parser->row, parser->col, msg.c_str());
			return;
		}

		try {
			check->SetOptions(parser->GetParserOptions());
			check->CheckFile(file);
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s\n", check->row, check->col, msg.c_str());
		}
	}

} // namespace internal
}