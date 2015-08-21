#include "script.h"
#include <stdio.h> // printf doesn't work without this
#include "cobra/mem/isolate.h"
#include "../include/Cobra.h"

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
			isolate->GetContext()->AddToInProgress(absolutePath);
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

		if (compiled){
			isolate->GetContext()->AddScript(this);
			isolate->GetContext()->RemoveFromInProgress(absolutePath.c_str());
		}
	}

	void Script::SetIncludes(std::vector<ASTInclude*> includes){
		for (int i = 0; i < includes.size(); i++){
			ASTInclude* include = includes[i];
			std::string name = include->name;
			std::string importPath = GetPathOfImport(name);
			if (!isolate->GetContext()->IsIncluded(isolate, importPath.c_str())){
				Cobra::Isolate* iso = CAST(Cobra::Isolate*, isolate);
				Cobra::Handle* includeStr = Cobra::String::NewFromFile(iso, importPath.c_str());
				Cobra::Handle* scr = Cobra::Script::Compile(iso, includeStr);
			}		
		}
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
	std::string Script::GetPathOfImport(std::string import){
		if (import.find("/") == std::string::npos){ // test2.cb
			std::string r = absolutePath;
			String::Replace(r, GetFileName(), import);
			return r;
		}
		else{
			if (import.find("./") == std::string::npos){ // sub/test3.cb
				std::string r = absolutePath;
				String::Replace(r, GetFileName(), import);
				return r;
			}
			else {
				std::string r = absolutePath;
				return ParseRelativePath(r, import);
			}
		}
	}

	std::string Script::ParseRelativePath(std::string absolute, std::string path){
		int dots = 0;
		for (int i = 0; i < path.length(); i++){
			if (path[i] == '.') dots++;
			else if (path[i] == '/'){
				switch (dots){
					case 2: {
						absolute = BackFolder(absolute);
						break;
					}
					case 1: {
						absolute = StayInFolder(absolute);
						break;
					}
					default: {
						return "";
					}
				}
				dots = 0;
			}
			else{
				std::string r = absolute;
				r += path.substr(i);
				return r;
			}
		}
		return "";
	}

	std::string Script::BackFolder(std::string path){
		std::size_t found = path.find_last_of("/\\");
		path = path.substr(0, found);
		found = path.find_last_of("/\\");
		return path.substr(0, found + 1);
	}

	std::string Script::StayInFolder(std::string path){
		std::size_t found = path.find_last_of("/\\");
		return path.substr(0, found + 1);
	}

} // namespace internal
}