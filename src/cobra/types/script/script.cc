#include "script.h"
#include <stdio.h> // printf doesn't work without this
#include "cobra/mem/isolate.h"
#include "../include/Cobra.h"
#include "cobra/flags.h"
#include "cobra/error/error.h"

namespace Cobra {
namespace internal{

	Script::Script(Handle* handle, Isolate* isolate){
		source = handle;
		parser = NULL;
		check = NULL;
		hasErr = false;
		this->isolate = isolate;
		compiled = false;
		parsingTime = PARSING_TIME;
	}

	// TODO:
	// 		Add TRY_CATCH
	// 		Pass Isolate through
	void Script::Compile(){
		std::string sourceCode = "";
		String* string = source->ToString();
		sourceCode += string->GetValue();
		absolutePath = string->GetPath();
		
		parser = source->isolate->InsertToHeap(new Parser(&sourceCode, &absolutePath), PARSER);
		parser->SetIsolate(source->isolate);

		check = source->isolate->InsertToHeap(new Check(), CHECK);
		check->SetIsolate(source->isolate);
		std::map<std::string, int> includes;
		ASTFile* main = NULL;
		Clock* clock = NULL;

		try {
			if (parsingTime) {
				clock = new Clock;
				clock->Start();
			}

			main = parser->Parse();

			if (parsingTime) {
				clock->Stop();
				printf("Parsing:  %f sec | %s\n", clock->GetDuration(), parser->filePath->c_str());
			}

			isolate->GetContext()->AddToInProgress(absolutePath);
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, parser->expected);
			msg = std::to_string(parser->Row) + ":" + std::to_string(parser->Col) + " - " + msg + " - \n\t" + absolutePath.c_str() + "\n\n" + GetSourceRow(parser->Row, parser->Col);
			msgs.push_back(msg.c_str());
			printf("%s\n", msg.c_str());
			hasErr = true;
			return;
		}

		try {
			SetIncludes();
		}
		catch (Error::ERROR e){
			return;
		}

		try {
			if (parsingTime) {
				clock->Reset();
				clock->Start();
			}
			check->SetMode(parser->mode);
			check->CheckFile(main);
			if (parsingTime) {
				clock->Stop();
				printf("Checking: %f sec | %s\n", clock->GetDuration(), parser->filePath->c_str());
				delete clock;
			}

			compiled = true;
		}
		catch (Error::ERROR e){	
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s - \n\t%s\nCode:\n\n%s\n", check->row, check->col, msg.c_str(), absolutePath.c_str(), GetSourceRow(check->row, check->col).c_str());
		}

		if (compiled){
			isolate->GetContext()->AddScript(this);
			isolate->GetContext()->RemoveFromInProgress(absolutePath.c_str());
		}
		else{
			printf("Did not compile: %s\n", absolutePath.c_str());
		}
	}

	void Script::SetIncludes(){
		for (int i = 0; i < parser->includes.size(); i++){
			ASTInclude* include = parser->includes[i];
			std::string name = include->name;
			std::string importPath = GetPathOfImport(name);
			include->name = importPath;
			if (!isolate->GetContext()->IsIncluded(isolate, importPath.c_str())){
				Cobra::Isolate* iso = CAST(Cobra::Isolate*, isolate);
				Cobra::Handle* includeStr = Cobra::String::NewFromFile(iso, importPath.c_str());
				Cobra::Handle* scr = Cobra::Script::Compile(iso, includeStr);
				if (scr->IsCompiled()){throw Error::COMPILATION_ERROR;}
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

	ASTNode* Script::GetExportedObject(std::string name){
		for (int i = 0; i < parser->exports.size(); i++){
			if (parser->exports[i]->name == name){
				return parser->exports[i];
			}
		}
		return NULL;
	}

	std::string Script::GetSourceRow(int row, int col){
		std::string sourceCode = "";
		String* string = source->ToString();
		sourceCode += string->GetValue();
		std::string result = "";
		std::string u = UNDERLINE_START;
		std::string u2 = UNDERLINE_STOP;
		int r = 0;
		int c = 0;
		bool carot = false;
		for (int i = 0; i < sourceCode.length(); i++){
			if (sourceCode[i] == '\t'){
				sourceCode[i] = ' ';
			}
			if (r == row - 1){
				result += sourceCode[i];
			}
			else if (r == row){
				if (!carot){
					for (int j = 0; j < col + 1; j++){
						if (j == col){
							result += "^\n";
						}
						else{
							result += " ";
						}
					}
					carot = true;
				}
				result += sourceCode[i];
			}
			else if (r > row) break;
			if (sourceCode[i] == '\n'){
				r++;
				c = 0;
			}
			c++;
		}
		return result;
	}

	int Script::CharsToNewLine(std::string code, int start){
		int count = 0;
		for (int i = start; i < code.length(); i++){
			if (code[i] == '\n') break;
			count++;
		}	
		return count;
	}

	void Script::Run(){
		
	}

} // namespace internal
}