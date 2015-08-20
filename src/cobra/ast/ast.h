#ifndef AST_H_
#define AST_H_

#include <string>
#include <map>
#include <vector>
#include "cobra/token/token.h"
#include "cobra/scanner/scanner.h"
#include "scope.h"

namespace Cobra {
namespace internal{

	class Scope;

	enum VISIBILITY{
		vPUBLIC, // default
		vPRIVATE,
		vPROTECTED,
		vSTATIC,
		vCONSTRUCTOR
	};

	class ASTNode
	{
	public:
		ASTNode(){type = ILLEGAL;visibility = vPUBLIC;scan = true;row = 0; col = 0;}
		~ASTNode(){}
		std::string name;
		TOKEN type;
		VISIBILITY visibility;
		bool scan;
		int row;
		int col;
	};

	class ASTExpr : public ASTNode
	{
	public:
		ASTExpr(){type = EXPR; value = NULL;}
		~ASTExpr(){}
		ASTExpr* value;
		TOKEN assignType;
	};

	class ASTIdent : public ASTExpr
	{
	public:
		ASTIdent(){type = IDENT;value = NULL;inc = false;dec = false;pre = false;post = false;}
		~ASTIdent(){}
		int pos;
		bool inc;
		bool dec;
		bool pre;
		bool post;
		ASTNode* value;
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
		ASTUnaryExpr(){type = UNARY;value = NULL; op = NULL;}
		~ASTUnaryExpr(){}
		ASTExpr* value;
		Token* op;
		int pos;
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		ASTBinaryExpr(){type = BINARY;Left = NULL; Right = NULL; op = NULL;}
		~ASTBinaryExpr(){}
		ASTExpr* Left;
		ASTExpr* Right;
		Token* op;
	};

	class ASTArrayMemberExpr : public ASTExpr
	{
	public:
		ASTArrayMemberExpr(){type = ARRAY_MEMBER;member = NULL; value = NULL;}
		~ASTArrayMemberExpr(){}
		ASTExpr* member;
		ASTExpr* value;
	};

	class ASTObjectMemberChainExpr : public ASTExpr
	{
	public:
		ASTObjectMemberChainExpr(){type = OBJECT_MEMBER_CHAIN; isSetting = false;member = NULL; object = NULL; value = NULL; isSetting = false;}
		~ASTObjectMemberChainExpr(){}
		ASTExpr* member;
		ASTIdent* object;
		ASTExpr* value;
		bool isSetting;
	};

	class ASTVar : public ASTNode
	{
	public:
		ASTVar(){type = VAR;stmt = NULL;varClass = NULL;}
		~ASTVar(){}
		ASTNode* stmt;
		TOKEN varType;
		TOKEN arrayType;
		ASTIdent* varClass;
	};

	class ASTVarList : public ASTNode
	{
	public:
		ASTVarList(){type = VARLIST;}
		~ASTVarList(){}
		std::vector<ASTVar*> vars;
	};

	// Literals
	class ASTInt : public ASTNode
	{
	public:
		ASTInt(){type = INT;value = 0;}
		~ASTInt(){}
		int value;
	};

	class ASTFloat : public ASTNode
	{
	public:
		ASTFloat(){type = FLOAT;value = 0.0;}
		~ASTFloat(){}
		float value;
	};

	class ASTDouble : public ASTNode
	{
	public:
		ASTDouble(){type = DOUBLE;value = 0.0;}
		~ASTDouble(){}
		double value;
	};

	class ASTBoolean : public ASTNode
	{
	public:
		ASTBoolean(){type = BOOLEAN;value = false;}
		~ASTBoolean(){}
		bool value;
	};

	class ASTChar : public ASTNode
	{
	public:
		ASTChar(){type = CHAR;value = '\0';}
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
		ASTBlock(){type = BLOCK;scope = NULL;}
		~ASTBlock(){}
		Scope* scope;
	};

	class ASTFunc : public ASTNode
	{
	public:
		ASTFunc(){type = FUNC;body = NULL;}
		~ASTFunc(){}
		ASTBlock* body;
		std::map<std::string, ASTNode*> args;
		std::vector<ASTNode*> ordered;
	};

	class ASTFuncCallExpr : public ASTExpr
	{
	public:
		ASTFuncCallExpr(){type = FUNC_CALL;isNew = false;pos = 0; func = NULL; scope = NULL;}
		~ASTFuncCallExpr(){}
		std::vector<ASTExpr*> params;
		int pos;
		bool isNew;
		ASTFunc* func;
		Scope* scope;
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
		~ASTObject(){}
		std::map<std::string, ASTNode*> members;
	};

	class ASTIf : public ASTNode
	{
	public:
		ASTIf(){type = IF;conditions = NULL; block = NULL;}
		~ASTIf(){}
		ASTExpr* conditions;
		ASTBlock* block;
	};

	class ASTElse : public ASTNode
	{
	public:
		ASTElse(){type = ELSE;ifStmt = NULL; conditions = NULL; block = NULL; ifStmt = NULL;}
		~ASTElse(){}
		ASTExpr* conditions;
		ASTBlock* block;
		ASTIf* ifStmt;
	};

	class ASTWhile : public ASTNode
	{
	public:
		ASTWhile(){type = WHILE;conditions = NULL; block = NULL;}
		~ASTWhile(){}
		ASTExpr* conditions;
		ASTBlock* block;
	};

	class ASTFor : public ASTNode
	{
	public:
		ASTFor(){type = FOR;var = NULL; conditions = NULL; iterator = NULL; block = NULL;};
		~ASTFor(){}
		ASTNode* var;
		ASTExpr* conditions;
		ASTExpr* iterator;
		ASTBlock* block;
	};

	class ASTFile : public ASTNode
	{
	public:
		ASTFile(){type = FILE;scope = NULL;}
		~ASTFile(){}
		Scope* scope;
	};

	class ASTImport : public ASTNode
	{
	public:
		ASTImport(){type = IMPORT;}
		~ASTImport(){}
		std::string name;
		std::string alias;
	};

	class ASTInclude : public ASTNode
	{
	public:
		ASTInclude(){file = NULL;type = INCLUDE;}
		~ASTInclude(){}
		std::string name;
		std::string alias;
		ASTFile* file;
	};
} // namespace internal
}

#endif // AST_H_
