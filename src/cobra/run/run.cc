#include "run.h"

namespace Cobra {

	Run::Run(std::string source){
		parser = new Parser(source);
		check = new Check;
	}

	Run::~Run(){
		delete parser;
	}

	void Run::Start(){
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
			//Init();
		}
		catch (Error::ERROR e){
			std::string msg = Error::String(e, NULL);
			printf("%d:%d - %s\n", check->row, check->col, msg.c_str());
		}
	}

	void Run::Init(){
		if (HasFunction("main")){
			ASTFunc* func = (ASTFunc*) GetObject("main");
			CallFunction(func);
		}
	}

	ASTNode* Run::GetObject(std::string name){
		ASTFile* file = files["main"];
		return file->scope->Lookup(name);
	}

	void Run::CallFunction(ASTFunc* func){
		if (func->body == NULL){} // do nothing

	}

	bool Run::HasObjectInScope(std::string name, Scope* scope){
		ASTNode* node = scope->Lookup(name);
		return node != NULL;
	}

	bool Run::HasFunction(std::string name){
		ASTFile* file = files["main"];
		ASTNode* func = file->scope->Lookup(name);
		return func != NULL && func->type == FUNC;
	}

} // namespace Cobra

void Compile(std::string source){
	Cobra::Run* run = new Cobra::Run(source);
	run->Start();
}