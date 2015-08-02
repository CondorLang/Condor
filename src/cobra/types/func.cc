#include "func.h"

namespace Cobra {

	FuncArg::FuncArg(){

	}

	FuncArg::~FuncArg(){
		delete value;
	}

	void FuncArg::SetAST(ASTNode* node){
		if (node == NULL) throw Error::CANT_HAVE_NULL_FUNC_ARG;
		value = node;
		name = value->name;
	}

	std::string FuncArg::GetName(){
		return value->name;
	}

	Func::Func(){
		ast = NULL;
		kPrivate = false;
		kPublic = true;
		kStatic = false;
		kAlone = true;
	}
	Func::~Func(){
		delete ast;
	}

	void Func::Call(std::vector<Type*>* callArgs, FuncReturnValue* val){
		if (callArgs == NULL) {} // do nothing now	
		printf("%s\n", "Calling function");
	}

	void Func::SetAST(ASTFunc* node){
		if (node == NULL) throw Error::UNDEFINED_FUNC;
		ast = node;
		name = node->name;

		FuncArg* arg = NULL;
		for (int i = 0; i < ast->args.size(); i++){
			arg = new FuncArg;
			arg->SetAST(ast->ordered[i]);
			args.push_back(arg);
		}
	}

	bool Func::IsCallable(){
		return ast->body != NULL;
	}

	bool Func::HasReturn(){
		return ast->body->scope->Lookup("return") != NULL;
	}
} // namespace Cobra