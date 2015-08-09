#include "script.h"
#include <stdio.h> // printf doesn't work without this
#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	Script::Script(Handle* handle){
		source = handle;
		parser = NULL;
		check = NULL;
		hasErr = false;
	}

	// TODO:
	// 		Add TRY_CATCH
	// 		Pass Isolate through
	void Script::Compile(){
		std::string sourceCode = "";
		String* string = source->ToString();
		sourceCode += string->GetValue();
		
		parser = new Parser(&sourceCode);
		parser->SetIsolate(source->isolate);

		check = new Check();
		check->SetIsolate(source->isolate);
		ASTFile* file = NULL;

		try {
			file = parser->Parse();
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, parser->expected);
			msg = std::to_string(parser->row) + ":" + std::to_string(parser->col) + " - " + msg;
			msgs.push_back(msg.c_str());
			printf("%s\n", msg.c_str());
			hasErr = true;
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