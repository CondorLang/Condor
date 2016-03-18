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
		static ASTVar* New(Isolate* iso);
		
	};

	class ASTFunc : public ASTNode
	{
	public:
		Scope* scope;
		std::vector<ASTVar*> args;
		static ASTFunc* New(Isolate* iso);
	};

} // namespace internal
}

#endif // NODE_H_
