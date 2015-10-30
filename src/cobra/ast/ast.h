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

	/**
	 * Parser mode is either lazy or strict
	 */
	enum P_MODE {
		STRICT,
		LAZY
	};

	class ASTNode
	{
	public:
		ASTNode(){}
		~ASTNode(){}
		std::string name;
		TOKEN type;
		VISIBILITY visibility;
		bool scan;
		int row;
		int col;
		bool used;
		static ASTNode* New(Isolate* iso);
		static void SetDefaults(ASTNode* node);
	};

	class ASTExpr : public ASTNode
	{
	public:
		ASTExpr(){}
		~ASTExpr(){}
		ASTExpr* value;
		TOKEN assignType;
		static ASTExpr* New(Isolate* iso);
	};

	class ASTNot : public ASTExpr
	{
	public:
		ASTNot(){}
		~ASTNot(){}
		ASTExpr* value;
		static ASTNot* New(Isolate* iso);
	};

	class ASTUndefined : public ASTExpr
	{
	public:
		ASTUndefined(){}
		~ASTUndefined(){}
		static ASTUndefined* New(Isolate* iso);
	};

	class ASTNull : public ASTExpr
	{
	public:
		ASTNull(){}
		~ASTNull(){}		
		static ASTNull* New(Isolate* iso);
	};

	class ASTCastExpr : public ASTExpr
	{
	public:
		ASTCastExpr(){}
		~ASTCastExpr(){}
		TOKEN castType;
		ASTExpr* to;
		ASTExpr* value;
		static ASTCastExpr* New(Isolate* iso);
	};

	class ASTIdent : public ASTExpr
	{
	public:
		ASTIdent(){}
		~ASTIdent(){}
		int pos;
		bool inc;
		bool dec;
		bool pre;
		bool post;
		ASTNode* value;
		static ASTIdent* New(Isolate* iso);
	};

	class ASTLiterary : public ASTExpr
	{
	public:
		ASTLiterary(){}
		~ASTLiterary(){}
		int pos;
		std::string value;
		TOKEN kind;
		static ASTLiterary* New(Isolate* iso);
	};

	class ASTUnaryExpr : public ASTExpr
	{
	public:
		ASTUnaryExpr(){}
		~ASTUnaryExpr(){}
		ASTExpr* value;
		Token* op;
		int pos;
		static ASTUnaryExpr* New(Isolate* iso);
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		ASTBinaryExpr(){}
		~ASTBinaryExpr(){}
		ASTExpr* Left;
		ASTExpr* Right;
		Token* op;
		static ASTBinaryExpr* New(Isolate* iso);
	};

	class ASTArrayMemberExpr : public ASTExpr
	{
	public:
		ASTArrayMemberExpr(){}
		~ASTArrayMemberExpr(){}
		ASTExpr* member;
		ASTExpr* value;
		static ASTArrayMemberExpr* New(Isolate* iso);
	};

	class ASTObjectMemberChainExpr : public ASTExpr
	{
	public:
		ASTObjectMemberChainExpr(){}
		~ASTObjectMemberChainExpr(){}
		ASTExpr* member;
		ASTNode* object;
		ASTExpr* value;
		bool isSetting;
		static ASTObjectMemberChainExpr* New(Isolate* iso);
	};

	class ASTVar : public ASTNode
	{
	public:
		ASTVar(){}
		~ASTVar(){}
		ASTExpr* stmt;
		TOKEN varType;
		TOKEN arrayType;
		bool array;
		ASTNode* varClass;
		bool cast;
		TOKEN castType;
		static ASTVar* New(Isolate* iso);
	};

	class ASTVarList : public ASTNode
	{
	public:
		ASTVarList(){}
		~ASTVarList(){}
		std::vector<ASTVar*> vars;
		static ASTVarList* New(Isolate* iso);
	};

	// Literals
	class ASTInt : public ASTNode
	{
	public:
		ASTInt(){}
		~ASTInt(){}
		int value;
		static ASTInt* New(Isolate* iso);
	};

	class ASTFloat : public ASTNode
	{
	public:
		ASTFloat(){}
		~ASTFloat(){}
		float value;
		static ASTFloat* New(Isolate* iso);
	};

	class ASTDouble : public ASTNode
	{
	public:
		ASTDouble(){}
		~ASTDouble(){}
		double value;
		static ASTDouble* New(Isolate* iso);
	};

	class ASTBoolean : public ASTNode
	{
	public:
		ASTBoolean(){}
		~ASTBoolean(){}
		bool value;
		static ASTBoolean* New(Isolate* iso);
	};

	class ASTChar : public ASTNode
	{
	public:
		ASTChar(){}
		~ASTChar(){}
		char value;
		static ASTChar* New(Isolate* iso);
	};

	class ASTString : public ASTNode
	{
	public:
		ASTString(){}
		~ASTString(){}
		std::string value;
		static ASTString* New(Isolate* iso);
	};

	class ASTBlock : public ASTNode
	{
	public:
		ASTBlock(){}
		~ASTBlock(){}
		Scope* scope;
		static ASTBlock* New(Isolate* iso);
	};

	class ASTFunc : public ASTNode
	{
	public:
		ASTFunc(){}
		~ASTFunc(){}
		ASTBlock* body;
		std::vector<ASTVar*> args;
		TOKEN returnType;
		bool used;
		static ASTFunc* New(Isolate* iso);
	};

	class ASTObject : public ASTNode
	{
	public:
		ASTObject(){}
		~ASTObject(){}
		std::vector<ASTNode*> members;
		static ASTObject* New(Isolate* iso);	
	};

	class ASTFuncCallExpr : public ASTExpr
	{
	public:
		ASTFuncCallExpr(){}
		~ASTFuncCallExpr(){}
		std::vector<ASTExpr*> params;
		int pos;
		bool isNew;
		ASTFunc* func;
		Scope* scope;
		TOKEN assignType;
		static ASTFuncCallExpr* New(Isolate* iso);
	};

	class ASTObjectInit : public ASTFuncCallExpr
	{
	public:
		ASTObjectInit(){}
		~ASTObjectInit(){}	
		ASTObject* base;
		std::vector<ASTNode*> members;
		static ASTObjectInit* New(Isolate* iso);	
	};

	class ASTArray : public ASTVar
	{
	public:
		ASTArray(){}
		~ASTArray(){}
		std::vector<ASTNode*> value;
		TOKEN arrayType;
		ASTObjectInit* base;
		static ASTArray* New(Isolate* iso, TOKEN rType);	
	};

	class ASTIf : public ASTNode
	{
	public:
		ASTIf(){}
		~ASTIf(){}
		ASTExpr* conditions;
		ASTBlock* block;
		static ASTIf* New(Isolate* iso);	
	};

	class ASTElse : public ASTNode
	{
	public:
		ASTElse(){}
		~ASTElse(){}
		ASTExpr* conditions;
		ASTBlock* block;
		ASTIf* ifStmt;
		static ASTElse* New(Isolate* iso);	
	};

	class ASTWhile : public ASTNode
	{
	public:
		ASTWhile(){}
		~ASTWhile(){}
		ASTExpr* conditions;
		ASTBlock* block;
		static ASTWhile* New(Isolate* iso);
	};

	class ASTFor : public ASTNode
	{
	public:
		ASTFor(){}
		~ASTFor(){}
		ASTNode* var;
		ASTExpr* conditions;
		ASTExpr* iterator;
		ASTBlock* block;
		static ASTFor* New(Isolate* iso);
	};

	class ASTTryCatch : public ASTNode
	{
	public:
		ASTTryCatch(){}
		~ASTTryCatch(){}
		ASTBlock* tryBlock;
		ASTFunc* catchFunc;
		static ASTTryCatch* New(Isolate* iso);
	};

	class ASTThrow : public ASTNode
	{
	public:
		ASTThrow(){}
		~ASTThrow();
		ASTExpr* obj;
		static ASTThrow* New(Isolate* iso);
	};

	class ASTImport : public ASTNode
	{
	public:
		ASTImport(){}
		~ASTImport(){}
		std::string name;
		std::string alias;
		static ASTImport* New(Isolate* iso);
	};

	class ASTInclude : public ASTNode
	{
	public:
		ASTInclude(){}
		~ASTInclude(){}
		std::string name;
		std::string alias;
		static ASTInclude* New(Isolate* iso);
	};

	class ASTFile : public ASTNode
	{
	public:
		ASTFile(){}
		~ASTFile(){}
		Scope* scope; 
		std::map<std::string, ASTInclude*> includes;
		std::vector<ASTInclude*> includesList;
		static ASTFile* New(Isolate* iso);
	};
} // namespace internal
}

#endif // AST_H_
