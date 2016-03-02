#include "script.h"
#include <stdio.h> // printf doesn't work without this
#include "cobra/mem/isolate.h"
#include "../include/Cobra.h"
#include "cobra/flags.h"
#include "cobra/error/error.h"

namespace Cobra {
namespace internal{

	Script* Script::New(Handle* string, Isolate* isolate){
		void* p = (Script*) isolate->GetMemory(sizeof(Script));
		Script* script = new(p) Script();
		if (script == NULL) return NULL;
		script->SetDefualts(string, isolate);
		return script;
	}

	void Script::SetDefualts(Handle* string, Isolate* isolate){
		source = string;
		parser = NULL;
		check = NULL;
		hasErr = false;
		this->isolate = isolate;
		compiled = false;
		parsingTime = PARSING_TIME;
		internal = false;
		sourceCode = "";
		String* str = source->ToString();
		internal = str->IsInternal();
		sourceCode += str->GetValue();
		absolutePath = str->GetPath();
		msgs.SetIsolate(isolate);
		name = string->name;
	}

	// TODO:
	// 		Add TRY_CATCH
	void Script::Compile(){
		parser = Parser::New(isolate, &sourceCode, &absolutePath);
		bool isInline = strncmp(parser->filePath->c_str(), "inline", 6);

		parser->SetInteral(internal);
		parser->SetInline(isInline);

		check = Check::New(isolate);
		check->SetIsolate(isolate);
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
				if (isInline) 
					printf("Parsing:  %f sec | %s\n", clock->GetDuration(), parser->filePath->c_str());
			}
			if (absolutePath != "inline"){
				isolate->GetContext()->AddToInProgress(absolutePath);
			}
			else{
				isolate->GetContext()->AddToInProgress(name);
			}
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, parser->expected);
			msg = std::to_string(parser->Row) + ":" + std::to_string(parser->Col) + " - " + msg + " - \n\t" + absolutePath.c_str() + "\n\n" + GetSourceRow(parser->Row, parser->Col);
			msgs.push_back(msg);
			printf("%s\n", msg.c_str());
			hasErr = true;
			return;
		}

		try {
			SetIncludes();
			SetImports();
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s - \n\t%s\nCode:\n\n%s\n", check->row, check->col, msg.c_str(), absolutePath.c_str(), GetSourceRow(check->row, check->col).c_str());
			return;
		}

		try {
			if (parsingTime) {
				clock->Reset();
				clock->Start();
			}
			check->SetInline(isInline);
			check->SetMode(parser->mode);
			check->CheckFile(main);
			if (parsingTime) {
				clock->Stop();
				if (isInline) 
					printf("Checking: %f sec | %s\n", clock->GetDuration(), parser->filePath->c_str());
				delete clock;
			}

			compiled = true;
		}
		catch (Error::ERROR e){	
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s - \n\t%s\nCode:\n\n%s\n", check->row, check->col, msg.c_str(), absolutePath.c_str(), GetSourceRow(check->row, check->col).c_str());
			hasErr = true;
		}

		if (compiled){
			isolate->GetContext()->AddScript(this);
			if (absolutePath == "inline"){
				isolate->GetContext()->RemoveFromInProgress(name);
			}
			else{
				isolate->GetContext()->RemoveFromInProgress(absolutePath);
			}
		}
		else{
			printf("Did not compile: %s\n", absolutePath.c_str());
		}
		isolate->RunGC();
	}

	void Script::SetIncludes(){
		Vector<std::string> tmpIncludes;
		tmpIncludes.SetIsolate(isolate);
		for (int i = 0; i < parser->includes.size(); i++){
			ASTInclude* include = parser->includes[i];
			std::string name = include->name;
			std::string importPath = GetPathOfImport(name);
			include->name = importPath;
			if (!isolate->GetContext()->IsIncluded(isolate, importPath.c_str())){
				tmpIncludes.push_back(importPath);
			}		
		}
		
		for (int i = 0; i < tmpIncludes.size(); i++){
			std::string importPath = tmpIncludes[i];
			Cobra::Isolate* iso = CAST(Cobra::Isolate*, isolate);
			Cobra::Handle* includeStr = Cobra::String::NewFromFile(iso, importPath.c_str());
			Cobra::Handle* scr = Cobra::Script::Compile(iso, includeStr);
			if (scr->IsCompiled()){throw Error::COMPILATION_ERROR;}
		}
	}

	void Script::SetImports(){
		for (int i = 0; i < parser->imports.size(); i++){
			ASTImport* import = parser->imports[i];
			std::string name = import->name;
			if (!isolate->GetContext()->IsImported(isolate, name)){
				isolate->GetContext()->SetImport(name);
				if (name == "exception") Exception::CB(isolate);
				else if (name == "string") String::CB(isolate);
				else if (name == "array") Array::CB(isolate);
				else throw Error::INVALID_IMPORT;
			}		
		}
	}

	std::string Script::GetFileName(){
		std::size_t found = absolutePath.find_last_of("/\\");
		std::string fileName = absolutePath.substr(found+1);
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
		Script* s = (Script*) this; // needed for random bug, I don't understand this one
		if (s == NULL) return NULL;
		for (int i = 0; i < parser->exports.size(); i++){
			if (parser->exports[i]->name == name){
				return parser->exports[i];
			}
		}
		return NULL;
	}

	std::string Script::GetSourceRow(int row, int col){
		std::string src = "";
		String* string = source->ToString();
		src += string->GetValue();
		std::string result = "";
		std::string u = UNDERLINE_START;
		std::string u2 = UNDERLINE_STOP;
		int r = 0;
		int c = 0;
		bool carot = false;
		for (int i = 0; i < src.length(); i++){
			if (src[i] == '\t'){
				src[i] = ' ';
			}
			if (r == row - 1){
				result += src[i];
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
				result += src[i];
			}
			else if (r > row) break;
			if (src[i] == '\n'){
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

	void Script::RunInternalScript(Isolate* isolate, std::string hex, std::string _name){
		Cobra::Isolate* iso = CAST(Cobra::Isolate*, isolate);
		int len = hex.length();
		std::string newString;
		for(int i=0; i< len; i+=2)
		{
		    std::string byte = hex.substr(i,2);
		    char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
		    newString.push_back(chr);
		}
		Cobra::Handle* str = Cobra::String::New(iso, newString.c_str());
		Handle* iHandle = CAST(Handle*, str);
		iHandle->name = _name;
		String* iStr = iHandle->ToString();
		iStr->SetInternal();
		
		Cobra::Handle* script = Cobra::Script::Compile(iso, str);
		script->Run();
	}

	std::string Script::GetSourceCode(){
		return sourceCode;
	}

	void Script::PrintExported(){
		for (int i = 0; i < parser->exports.size(); i++){
			printf("%s - %s\n", absolutePath.c_str(), parser->exports[i]->name.c_str());
		}
	}

	void Script::Run(){
		
	}

} // namespace internal
}