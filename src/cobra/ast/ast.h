#ifndef AST_H_
#define AST_H_

#include <string>
#include <map>
#include <vector>
#include "../token/token.h"
#include "../scanner/scanner.h"
#include "scope.h"

namespace Cobra {

	class Scope;

	class ASTNode
	{
	public:
		ASTNode();
		~ASTNode(){}
		std::string name;
		TOKEN type;
	};

	// Literals
	class ASTInt : public ASTNode
	{
	public:
		ASTInt(){type = INT;}
		~ASTInt(){}
		int value;
	};

	class ASTFloat : public ASTNode
	{
	public:
		ASTFloat(){type = FLOAT;}
		~ASTFloat(){}
		float value;
	};

	class ASTDouble : public ASTNode
	{
	public:
		ASTDouble(){type = DOUBLE;}
		~ASTDouble(){}
		double value;
	};

	class ASTBoolean : public ASTNode
	{
	public:
		ASTBoolean(){type = BOOLEAN;}
		~ASTBoolean(){}
		bool value;
	};

	class ASTChar : public ASTNode
	{
	public:
		ASTChar(){type = CHAR;}
		~ASTChar(){}
		char value;
	};

	class ASTString : public ASTNode
	{
	public:
		ASTString(){type = STRING;}
		~ASTString(){}
		std::string value;
	};

	class ASTBlock : public ASTNode
	{
	public:
		ASTBlock(){}
		~ASTBlock(){}
		Scope* scope;
	};

	class ASTFunc : public ASTNode
	{
	public:
		ASTFunc(){type = FUNC;}
		~ASTFunc();
		std::string name;
		ASTBlock* body;
		std::map<std::string, ASTNode*> args;
	};

	class ASTArray : public ASTNode
	{
	public:
		ASTArray(TOKEN rType){type = ARRAY; arrayType = rType;}
		~ASTArray(){}
		std::vector<ASTNode*> value;
		TOKEN arrayType;
	};

	class ASTFile
	{
	public:
		ASTFile(){}
		~ASTFile();
		Scope* scope;
	};
}

#endif // AST_H_
