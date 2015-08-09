#include "func.h"

namespace Cobra {
namespace internal{

	FuncArg::FuncArg(){

	}

	FuncArg::~FuncArg(){
		//delete value;
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
		walk = NULL;
	}
	Func::~Func(){
		//delete ast;
		//if (walk != NULL) delete walk;
	}

	void Func::Call(std::vector<Type*>* callArgs, FuncReturnValue* val){
		if (callArgs == NULL) {} // do nothing now	
		else{
			for (int i = 0; i < callArgs->size(); i++){
				ASTLiterary* lit = Type::ToASTLiterary(callArgs->at(i));
				lit->name = args.at(i)->name;
				ast->body->scope->InsertBefore(lit);
			}
		}
		walk = new Walk();
		walk->WalkFunc(ast);
		ASTLiterary* lit = walk->GetValue();
		val->value = lit;
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
} // namespace internal
} // namespace Cobra