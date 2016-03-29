#include "script.h"
#include <stdio.h> // printf doesn't work without this

namespace Cobra {
namespace internal{

	Script* Script::New(Isolate* iso, String* str){
		void* p = (Script*) iso->GetMemory(sizeof(Script));
		Script* script = new(p) Script(iso, str);
		return script;
	}

	Script::Script(Isolate* isolate, String* str){
		source = str;
		hasErr = false;
		this->isolate = isolate;
		compiled = false;
		parser = NULL;
		parsingTime = PARSING_TIME;
		sourceCode = "";
		internal = str->IsInternal();
		sourceCode += str->GetValue();
		absolutePath = str->GetPath();
		name = absolutePath;
		semantics = NULL;
		currentCode = NULL;
	}

	void Script::RunInternalScript(Isolate* isolate, std::string hex, std::string _name){
		
	}

	void Script::Compile(){
		parser = Parser::New(isolate, &sourceCode);
		bool isInline = absolutePath == "inline";
		parser->SetInteral(internal);
		parser->SetInline(isInline);
		Clock* clock = NULL;
		try {
			if (parsingTime) {
				clock = new Clock;
				clock->Start();
			}

			parser->Parse();

			if (parsingTime) {
				clock->Stop();
				if (!parser->IsInternal()) 
					printf("Parsing:  %f sec | %s\n", clock->GetDuration(), absolutePath.c_str());
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
			currentCode = parser->GetSource();
			msg = std::to_string(parser->Row) + ":" + std::to_string(parser->Col) + " - " + msg + " - \n\t" + absolutePath.c_str() + "\n\n" + GetSourceRow(parser->Row, parser->Col);
			msgs.push_back(msg);
			printf("%s\n", msg.c_str());
			hasErr = true;
			return;
		}
		semantics = Semantics::New(isolate, parser);

		try {
			semantics->Evaluate(parser->GetBaseScope());
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, NULL);
			currentCode = semantics->GetSource();
			printf("%d:%d - %s - \n\t%s\nCode:\n\n%s\n", semantics->row, semantics->col, msg.c_str(), absolutePath.c_str(), GetSourceRow(semantics->row, semantics->col).c_str());
			hasErr = true;
		}

	}

	void Script::Run(){
		
	}

	std::string Script::GetSourceRow(int row, int col){
		std::string src = "";
		src += currentCode->c_str();
		std::string result = "";
		std::string u = UNDERLINE_START;
		std::string u2 = UNDERLINE_STOP;
		int r = 1;
		int c = 1;
		bool carrot = false;
		for (int i = 0; i < src.length(); i++){
			if (src[i] == '\t') src[i] = ' ';
			if (r == row - 1 || r == row){
				result += src[i];
			}
			else if (r > row) break;
			if (src[i] == '\n'){
				r++;
				c = 1;
			}
			c++;
		}
		result += '\n';
		for (int i = 0; i < col; i++){
			result += ' ';
		}
		result += "\033[1;32m^\033[0m";
		result += "\n";
		return result;
	}

} // namespace internal
}