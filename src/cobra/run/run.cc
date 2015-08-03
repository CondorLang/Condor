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
			Init();
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s\n", check->row, check->col, msg.c_str());
		}
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
		FuncReturnValue* returnValue = new FuncReturnValue;
		func->Call(NULL, returnValue);

		if (returnValue->value != NULL){
			printf("Main Called, returned: %s\n", returnValue->value->value.c_str());
		}
		else{
			printf("Working on\n");
		}
		//delete func;
		//delete returnValue;
	}

} // namespace Cobra

void Compile(std::string* source){
	Cobra::Run* run = new Cobra::Run();
	run->Compile(source);
	run->Execute();
	run->Dispose();
	delete run;
}