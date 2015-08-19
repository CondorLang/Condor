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

		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(sourceCode);
		
		parser = source->isolate->InsertToHeap(new Parser(&sourceCode), PARSER);
		parser->SetIsolate(source->isolate);

		check = source->isolate->InsertToHeap(new Check(), CHECK);
		check->SetIsolate(source->isolate);
		std::map<std::string, int> includes;
		ASTFile* main = NULL;

		try {
			main = parser->Parse();
			SetIncludes(parser->includes);
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
			check->CheckFile(main);
		}
		catch (Error::ERROR e){	
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s\n", check->row, check->col, msg.c_str());
		}
	}

	void Script::SetIncludes(std::vector<ASTInclude*> includes){
		for (int i = 0; i < includes.size(); i++){
			ASTInclude* include = includes[i];
			std::string name = include->name;
		}
	}

} // namespace internal
}