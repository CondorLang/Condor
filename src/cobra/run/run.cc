#include "run.h"

namespace Cobra {

	Run::Run(){
		parser = NULL;
		check = new Check;
	}

	Run::~Run(){

	}

	void Run::Compile(std::string* source){
		if (parser != NULL) delete parser;
		parser = new Parser(source);
	}

	void Run::Execute(){
		try {
			files["main"] = parser->Parse();
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, parser->expected);
			printf("%d:%d - %s\n", parser->row, parser->col, msg.c_str());
		}	

		try {
			check->SetOptions(parser->GetParserOptions());
			check->CheckFile(files["main"]);
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s\n", check->row, check->col, msg.c_str());
		}
		Init();
	}

	void Run::Dispose(){
		for (std::map<std::string, ASTFile*>::iterator it = files.begin(); it != files.end(); ++it){
			delete it->second;
		}
	}

	void Run::Init(){
		Type* main = files["main"]->scope->LookupType("main");
		if (main == NULL) return;
		Func* func = (Func*) main;
		FuncReturnValue* value = new FuncReturnValue;
		func->Call(NULL, value);
		
	}

} // namespace Cobra

void Compile(std::string* source){
	Cobra::Run* run = new Cobra::Run();
	run->Compile(source);
	run->Execute();
	run->Dispose();
	delete run;
}