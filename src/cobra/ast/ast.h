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
		ASTNode(){type = ILLEGAL;}
		~ASTNode(){}
		std::string name;
		TOKEN type;
	};

	class ASTExpr : public ASTNode
	{
	public:
		ASTExpr(){type = EXPR;}
		~ASTExpr();
		ASTExpr* value;
		TOKEN assignType;
	};

	class ASTIdent : public ASTExpr
	{
	public:
		ASTIdent(){type = IDENT;}
		~ASTIdent(){}
		int pos;
	};

	class ASTLiterary : public ASTExpr
	{
	public:
		ASTLiterary(){type = LITERARY;}
		~ASTLiterary(){}
		int pos;
		std::string value;
		TOKEN kind;
	};

	class ASTUnaryExpr : public ASTExpr
	{
	public:
		ASTUnaryExpr(){type = UNARY;}
		~ASTUnaryExpr();
		ASTExpr* value;
		Token* op;
		int pos;
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		ASTBinaryExpr(){type = BINARY;}
		~ASTBinaryExpr();
		ASTExpr* Left;
		ASTExpr* Right;
		Token* op;
	};

	class ASTFuncCallExpr : public ASTExpr
	{
	public:
		ASTFuncCallExpr(){type = FUNC_CALL;isNew = false;}
		~ASTFuncCallExpr();
		std::vector<ASTExpr*> params;
		int pos;
		bool isNew;
	};

	class ASTArrayMemberExpr : public ASTExpr
	{
	public:
		ASTArrayMemberExpr(){type = ARRAY_MEMBER;}
		~ASTArrayMemberExpr();
		ASTExpr* member;
		ASTExpr* value;
	};

	class ASTObjectMemberChainExpr : public ASTExpr
	{
	public:
		ASTObjectMemberChainExpr(){type = OBJECT_MEMBER_CHAIN; isSetting = false;}
		~ASTObjectMemberChainExpr();
		ASTExpr* member;
		ASTIdent* object;
		ASTExpr* value;
		bool isSetting;
	};

	class ASTVar : public ASTNode
	{
	public:
		ASTVar(){type = VAR;}
		~ASTVar();
		ASTNode* stmt;
		TOKEN varType;
		ASTIdent* varClass;
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
		ASTBlock(){type = BLOCK;}
		~ASTBlock(){}
		Scope* scope;
	};

	class ASTFunc : public ASTNode
	{
	public:
		ASTFunc(){type = FUNC;}
		~ASTFunc();
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

	class ASTObject : public ASTNode
	{
	public:
		ASTObject(){}
		~ASTObject();
		std::map<std::string, ASTExpr*> members;
	};

	class ASTIf : public ASTNode
	{
	public:
		ASTIf(){type = IF;}
		~ASTIf();
		ASTExpr* conditions;
		ASTBlock* block;
	};

	class ASTFile : public ASTNode
	{
	public:
		ASTFile(){type = FILE;}
		~ASTFile();
		Scope* scope;
	};
}

#endif // AST_H_
