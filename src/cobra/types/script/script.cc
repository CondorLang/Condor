#include "script.h"
#include <stdio.h> // printf doesn't work without this
#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	Script::Script(Handle* handle, Isolate* isolate){
		source = handle;
		parser = NULL;
		check = NULL;
		hasErr = false;
		this->isolate = isolate;
		compiled = false;
	}

	// TODO:
	// 		Add TRY_CATCH
	// 		Pass Isolate through
	void Script::Compile(){
		std::string sourceCode = "";
		String* string = source->ToString();
		sourceCode += string->GetValue();
		absolutePath = string->GetPath();
		
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
			compiled = true;
		}
		catch (Error::ERROR e){	
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s\n", check->row, check->col, msg.c_str());
		}

		if (compiled) isolate->GetContext()->AddScript(this);
	}

	void Script::SetIncludes(std::vector<ASTInclude*> includes){
		for (int i = 0; i < includes.size(); i++){
			ASTInclude* include = includes[i];
			std::string name = include->name;
			const char* importPath = GetPathOfImport(name);
			printf("d: %s\n", importPath);
		}
	}

	bool Script::Replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
	}

	std::string Script::GetFileName(){
		std::size_t found = absolutePath.find_last_of("/\\");
		std::string fileName = absolutePath.substr(found+1);
		printf("");
		return fileName;
	}

	/**
	 * TODO:
	 * 		windows support
	 */
	const char* Script::GetPathOfImport(std::string import){
		if (import.find("/") == std::string::npos){ // test2.cb
			std::string r = absolutePath;
			Replace(r, GetFileName(), import);
			return r.c_str();
		}
		return "";
	}

} // namespace internal
}