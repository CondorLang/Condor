#ifndef NODE_H_
#define NODE_H_

#include <string>

#include "cobra/token/token.h"
#include "cobra/mem/isolate.h"
#include "cobra/ast/scope.h"

namespace Cobra {
namespace internal{

	class Isolate;

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
		std::vector<TOKEN> visibility;
		static ASTNode* New(Isolate* iso);
		static void SetDefaults(ASTNode* node, Isolate* iso);
	};

	class ASTImport : public ASTNode
	{
	public:
		std::string alias;
		static ASTImport* New(Isolate* iso);
	};

	class ASTInclude : public ASTNode
	{
	public:
		std::string alias;
		static ASTInclude* New(Isolate* iso);
	};

	class ASTVar : public ASTNode
	{
	public:
		std::string baseName;
		TOKEN baseType;
		ASTNode* value;
		TOKEN assignmentType;
		static ASTVar* New(Isolate* iso);		
	};

	class ASTFunc : public ASTNode
	{
	public:
		Scope* scope;
		std::vector<ASTVar*> args;
		static ASTFunc* New(Isolate* iso);
	};

	class ASTExpr : public ASTNode
	{
	public:
		static ASTExpr* New(Isolate* iso);
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		static ASTBinaryExpr* New(Isolate* iso);
		ASTExpr* left;
		ASTExpr* right;
		TOKEN op;
		bool isBoolean;
	};

	class ASTLiteral : public ASTExpr
	{
	public:
		static ASTLiteral* New(Isolate* iso);
		std::string value;
		TOKEN litType;
		TOKEN unary;
		int isPost;
	};

	class ASTFuncCall : public ASTExpr
	{
	public:
		static ASTFuncCall* New(Isolate* iso);
		std::vector<ASTExpr*> params;	
		bool isInternal;	
	};

	class ASTForExpr : public ASTExpr
	{
	public:
		static ASTForExpr* New(Isolate* iso);
		ASTNode* var;
		ASTExpr* condition;
		ASTExpr* tick;
		Scope* scope; 		
	};

	class ASTWhileExpr : public ASTExpr
	{
	public:
		static ASTWhileExpr* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;		
	};

	class ASTTryCatchExpr : public ASTExpr
	{
	public:
		static ASTTryCatchExpr* New(Isolate* iso);
		Scope* tryScope;
		Scope* catchScope;
		std::vector<ASTVar*> catchParams;		
	};

	class ASTThrow : public ASTExpr
	{
	public:
		static ASTThrow* New(Isolate* iso);
		ASTExpr* expr;		
	};

	class ASTIf : public ASTExpr
	{
	public:
		static ASTIf* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;
		std::vector<ASTIf*> elseIfs;		
	};

	class ASTDelete : public ASTExpr
	{
	public:
		static ASTDelete* New(Isolate* iso);
		ASTNode* node;		
	};

	class ASTCase : public ASTExpr
	{
	public:
		static ASTCase* New(Isolate* iso);
		ASTNode* condition;
		Scope* scope;
		bool isDefault;
	};

	class ASTSwitch : public ASTExpr
	{
	public:
		static ASTSwitch* New(Isolate* iso);
		std::vector<ASTCase*> cases;
		ASTNode* value;
	};

	class ASTObject : public ASTNode
	{
	public:
		static ASTObject* New(Isolate* iso);
		Scope* scope;
	};

	class ASTUndefined : public ASTNode
	{
	public:
		static ASTUndefined* New(Isolate* iso);
	};

} // namespace internal
}

#endif // NODE_H_
