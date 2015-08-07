#ifndef FUNC_H_
#define FUNC_H_

#include <string>
#include <vector>
#include "type.h"
#include "cobra/ast/ast.h"
#include "cobra/error/error.h"
#include "cobra/ast/walk.h"

namespace Cobra {
namespace internal{

	class Walk;
	class Type;

	class FuncReturnValue
	{
	public:
		FuncReturnValue(){value = NULL;}
		~FuncReturnValue(){delete value;}
		ASTLiterary* value;
	};

	class FuncArg : public Type
	{
	private:
		ASTNode* value;
	public:
		FuncArg();
		~FuncArg();
		void SetAST(ASTNode* node);
		std::string GetName();
	};

	class Func : public Type
	{
	private:
		ASTFunc* ast;
		bool kPrivate;
		bool kPublic; 
		bool kStatic;
		bool kAlone;
		std::vector<FuncArg*> args;
		Walk* walk;

	public:
		Func();
		~Func();
		void SetAST(ASTFunc* node);
		void Call(std::vector<Type*>* callArgs, FuncReturnValue* val);
		bool IsPrivate(){return kPrivate;}
		bool IsPublic(){return kPublic;}
		bool IsStatic(){return kStatic;}
		bool HasArgs(){return args.size() > 0;}
		FuncArg* GetArg(int idx){if (args.size() < idx){return args[idx];} else{return NULL;}}
		bool IsCallable();
		bool IsAlone(){return kAlone;}
		bool IsInsideObject(){return !kAlone;}
		bool HasReturn();
	};
} // namespace internal{
}

#endif // FUNC_H_
