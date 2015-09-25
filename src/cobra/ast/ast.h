#ifndef AST_H_
#define AST_H_

#include <string>
#include <map>
#include "cobra/types/vector/vector.h"
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
		~ASTNode(){}
		std::string name;
		TOKEN type;
		VISIBILITY visibility;
		bool scan;
		int row;
		int col;
		bool used;
		static ASTNode* New(Isolate* iso);
	};

	class ASTExpr : public ASTNode
	{
	public:
		~ASTExpr(){}
		ASTExpr* value;
		TOKEN assignType;
		static ASTExpr* New(Isolate* iso);
	};

	class ASTNull : public ASTExpr
	{
	public:
		~ASTNull(){}		
		static ASTNull* New(Isolate* iso);
	};

	class ASTCastExpr : public ASTExpr
	{
	public:
		~ASTCastExpr(){}
		TOKEN castType;
		ASTExpr* to;
		ASTExpr* value;
		static ASTCastExpr* New(Isolate* iso);
	};

	class ASTIdent : public ASTExpr
	{
	public:
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
		~ASTLiterary(){}
		int pos;
		std::string value;
		TOKEN kind;
		static ASTLiterary* New(Isolate* iso);
	};

	class ASTUnaryExpr : public ASTExpr
	{
	public:
		~ASTUnaryExpr(){}
		ASTExpr* value;
		Token* op;
		int pos;
		static ASTUnaryExpr* New(Isolate* iso);
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		~ASTBinaryExpr(){}
		ASTExpr* Left;
		ASTExpr* Right;
		Token* op;
		static ASTBinaryExpr* New(Isolate* iso);
	};

	class ASTArrayMemberExpr : public ASTExpr
	{
	public:
		~ASTArrayMemberExpr(){}
		ASTExpr* member;
		ASTExpr* value;
		static ASTArrayMemberExpr* New(Isolate* iso);
	};

	class ASTObjectMemberChainExpr : public ASTExpr
	{
	public:
		~ASTObjectMemberChainExpr(){}
		ASTExpr* member;
		ASTIdent* object;
		ASTExpr* value;
		bool isSetting;
		static ASTObjectMemberChainExpr* New(Isolate* iso);
	};

	class ASTVar : public ASTNode
	{
	public:
		~ASTVar(){}
		ASTExpr* stmt;
		TOKEN varType;
		TOKEN arrayType;
		ASTIdent* varClass;
		bool cast;
		TOKEN castType;
		static ASTVar* New(Isolate* iso);
	};

	class ASTParamVar : public ASTVar
	{
	public:
		~ASTParamVar(){}	
		static ASTParamVar* New(Isolate* iso);	
	};

	class ASTVarList : public ASTNode
	{
	public:
		~ASTVarList(){}
		Vector<ASTVar*> vars;
		static ASTVarList* New(Isolate* iso);
	};

	// Literals
	class ASTInt : public ASTNode
	{
	public:
		~ASTInt(){}
		int value;
		static ASTInt* New(Isolate* iso);
	};

	class ASTFloat : public ASTNode
	{
	public:
		~ASTFloat(){}
		float value;
		static ASTFloat* New(Isolate* iso);
	};

	class ASTDouble : public ASTNode
	{
	public:
		~ASTDouble(){}
		double value;
		static ASTDouble* New(Isolate* iso);
	};

	class ASTBoolean : public ASTNode
	{
	public:
		~ASTBoolean(){}
		bool value;
		static ASTBoolean* New(Isolate* iso);
	};

	class ASTChar : public ASTNode
	{
	public:
		~ASTChar(){}
		char value;
		static ASTChar* New(Isolate* iso);
	};

	class ASTString : public ASTNode
	{
	public:
		~ASTString(){}
		std::string value;
		static ASTString* New(Isolate* iso);
	};

	class ASTBlock : public ASTNode
	{
	public:
		~ASTBlock(){}
		Scope* scope;
		static ASTBlock* New(Isolate* iso);
	};

	class ASTFunc : public ASTNode
	{
	public:
		~ASTFunc(){}
		ASTBlock* body;
		Vector<ASTParamVar*> args;
		TOKEN returnType;
		bool used;
		static ASTFunc* New(Isolate* iso);
	};

	class ASTFuncCallExpr : public ASTExpr
	{
	public:
		~ASTFuncCallExpr(){}
		Vector<ASTExpr*> params;
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
		~ASTObjectInit(){}	
		static ASTObjectInit* New(Isolate* iso);	
	};

	class ASTArray : public ASTParamVar
	{
	public:
		~ASTArray(){}
		Vector<ASTNode*> value;
		TOKEN arrayType;
		static ASTArray* New(Isolate* iso, TOKEN rType);	
	};

	class ASTObject : public ASTNode
	{
	public:
		~ASTObject(){}
		Vector<ASTNode*> members;
		static ASTObject* New(Isolate* iso);	
	};

	class ASTIf : public ASTNode
	{
	public:
		~ASTIf(){}
		ASTExpr* conditions;
		ASTBlock* block;
		static ASTIf* New(Isolate* iso);	
	};

	class ASTElse : public ASTNode
	{
	public:
		~ASTElse(){}
		ASTExpr* conditions;
		ASTBlock* block;
		ASTIf* ifStmt;
		static ASTElse* New(Isolate* iso);	
	};

	class ASTWhile : public ASTNode
	{
	public:
		~ASTWhile(){}
		ASTExpr* conditions;
		ASTBlock* block;
		static ASTWhile* New(Isolate* iso);
	};

	class ASTFor : public ASTNode
	{
	public:
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
		~ASTTryCatch(){}
		ASTBlock* tryBlock;
		ASTFunc* catchFunc;
		static ASTTryCatch* New(Isolate* iso);
	};

	class ASTThrow : public ASTNode
	{
	public:
		~ASTThrow();
		ASTExpr* obj;
		static ASTThrow* New(Isolate* iso);
	};

	class ASTImport : public ASTNode
	{
	public:
		~ASTImport(){}
		std::string name;
		std::string alias;
		static ASTImport* New(Isolate* iso);
	};

	class ASTInclude : public ASTNode
	{
	public:
		~ASTInclude(){}
		std::string name;
		std::string alias;
		static ASTInclude* New(Isolate* iso);
	};

	class ASTFile : public ASTNode
	{
	public:
		~ASTFile(){}
		Scope* scope; 
		std::map<std::string, ASTInclude*> includes;
		Vector<ASTInclude*> includesList;
		static ASTFile* New(Isolate* iso);
	};
} // namespace internal
}

#endif // AST_H_
