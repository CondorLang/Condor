#ifndef NODE_H_
#define NODE_H_

#include <string>

#include "cobra/global.h"
#include "cobra/token/token.h"
#include "cobra/mem/isolate.h"
#include "cobra/ast/scope.h"
#include "cobra/semantics/internal.h"

namespace Cobra {
namespace internal{

	class Isolate;
	class ASTLiteral;
	class Scope;

	typedef void (*InternalFunctionCallback)(ASTNode* node);

	// TODO: Create a Expand() function which expands the node and the tree
	class ASTNode
	{
	public:
		ASTNode(){}
		~ASTNode(){}
		std::string name;
		TOKEN type;
		int row;
		int col;
		int id;
		bool isExport;
		bool isInParen;
		std::vector<TOKEN> visibility;
		std::vector<ASTNode*> local;
		static ASTNode* New(Isolate* iso);
		static void SetDefaults(ASTNode* node, Isolate* iso);
		bool HasVisibility(TOKEN tok);
		size_t Size(){return sizeof(ASTNode);}
	};

	class ASTImport : public ASTNode
	{
	public:
		std::string alias;
		static ASTImport* New(Isolate* iso);
		size_t Size(){return sizeof(ASTImport);}
	};

	class ASTInclude : public ASTNode
	{
	public:
		std::string alias;
		static ASTInclude* New(Isolate* iso);
		size_t Size(){return sizeof(ASTInclude);}
	};

	class ASTVar : public ASTNode
	{
	public:
		std::string baseName;
		TOKEN baseType;
		ASTNode* value;
		ASTNode* member;
		TOKEN assignmentType;
		bool isArray;
		bool isObject;
		bool isArg;
		int order;
		static ASTVar* New(Isolate* iso);		
		size_t Size(){return sizeof(ASTVar);}
	};

	class ASTFunc : public ASTNode
	{
	public:
		Scope* scope;
		std::vector<ASTVar*> args;
		TOKEN assignmentType;
		static ASTFunc* New(Isolate* iso);
		size_t Size(){return sizeof(ASTFunc);}
	};

	class ASTExpr : public ASTNode
	{
	public:
		static ASTExpr* New(Isolate* iso);
		ASTLiteral* cast;
		size_t Size(){return sizeof(ASTExpr);}
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		static ASTBinaryExpr* New(Isolate* iso);
		ASTExpr* left;
		ASTExpr* right;
		TOKEN op;
		bool isBoolean;
		bool isChain;
		size_t Size(){return sizeof(ASTBinaryExpr);}
	};

	class ASTLiteral : public ASTExpr
	{
	public:
		static ASTLiteral* New(Isolate* iso);
		std::string value;
		ASTVar* var;
		TOKEN litType;
		TOKEN unary;
		bool isCast;
		int isPost;
		size_t Size(){return sizeof(ASTLiteral);}
	};

	class ASTFuncCall : public ASTExpr
	{
	public:
		static ASTFuncCall* New(Isolate* iso);
		std::vector<ASTExpr*> params;	
		bool isInternal;	
		bool isInit;
		ASTFunc* func;
		InternalFunctionCallback callback;
		size_t Size(){return sizeof(ASTFuncCall);}
	};

	class ASTForExpr : public ASTExpr
	{
	public:
		static ASTForExpr* New(Isolate* iso);
		ASTNode* var;
		ASTExpr* condition;
		ASTExpr* tick;
		Scope* scope; 		
		size_t Size(){return sizeof(ASTForExpr);}
	};

	class ASTWhileExpr : public ASTExpr
	{
	public:
		static ASTWhileExpr* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;		
		size_t Size(){return sizeof(ASTWhileExpr);}
	};

	class ASTTryCatchExpr : public ASTExpr
	{
	public:
		static ASTTryCatchExpr* New(Isolate* iso);
		Scope* tryScope;
		Scope* catchScope;
		std::vector<ASTVar*> catchParams;		
		size_t Size(){return sizeof(ASTTryCatchExpr);}
	};

	class ASTThrow : public ASTExpr
	{
	public:
		static ASTThrow* New(Isolate* iso);
		ASTExpr* expr;		
		size_t Size(){return sizeof(ASTThrow);}
	};

	class ASTIf : public ASTExpr
	{
	public:
		static ASTIf* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;
		std::vector<ASTIf*> elseIfs;	
		size_t Size(){return sizeof(ASTIf);}	
	};

	class ASTDelete : public ASTExpr
	{
	public:
		static ASTDelete* New(Isolate* iso);
		ASTNode* node;		
		size_t Size(){return sizeof(ASTDelete);}
	};

	class ASTCase : public ASTExpr
	{
	public:
		static ASTCase* New(Isolate* iso);
		ASTNode* condition;
		Scope* scope;
		bool isDefault;
		size_t Size(){return sizeof(ASTCase);}
	};

	class ASTSwitch : public ASTExpr
	{
	public:
		static ASTSwitch* New(Isolate* iso);
		std::vector<ASTCase*> cases;
		ASTNode* value;
		size_t Size(){return sizeof(ASTSwitch);}
	};

	class ASTObject : public ASTNode
	{
	public:
		static ASTObject* New(Isolate* iso);
		Scope* scope;
		bool extend;
		size_t Size(){return sizeof(ASTObject);}
	};

	class ASTUndefined : public ASTNode
	{
	public:
		static ASTUndefined* New(Isolate* iso);
		size_t Size(){return sizeof(ASTUndefined);}
	};

	class ASTArray : public ASTExpr
	{
	public:
		static ASTArray* New(Isolate* iso);
		std::vector<ASTExpr*> members;
		size_t Size(){return sizeof(ASTArray);}
	};

	class ASTObjectInstance : public ASTNode
	{
	public:
		static ASTObjectInstance* New(Isolate* iso);
		std::vector<ASTExpr*> members;
		ASTFuncCall* constructor;
		ASTObject* base;
		size_t Size(){return sizeof(ASTObjectInstance);}		
	};

} // namespace internal
}

#endif // NODE_H_
