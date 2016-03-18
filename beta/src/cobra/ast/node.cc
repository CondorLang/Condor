#include "node.h"

namespace Cobra {
namespace internal{

	ASTNode* ASTNode::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTNode));
		ASTNode* n = new(pt) ASTNode();
		ASTNode::SetDefaults(n, iso);
		return n;
	}

	void ASTNode::SetDefaults(ASTNode* node, Isolate* iso){
		node->row = 0;
		node->col = 0;
		node->type = UNDEFINED;
		node->name = "";
		node->id = iso->GetContext()->GetNextAstId();
	}

	ASTImport* ASTImport::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTImport));
		ASTImport* n = new(pt) ASTImport();
		ASTNode::SetDefaults(n, iso);
		n->type = IMPORT;
		return n;
	}

	ASTInclude* ASTInclude::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTInclude));
		ASTInclude* n = new(pt) ASTInclude();
		ASTNode::SetDefaults(n, iso);
		n->type = INCLUDE;
		return n;
	}

	ASTFunc* ASTFunc::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTFunc));
		ASTFunc* n = new(pt) ASTFunc();
		ASTNode::SetDefaults(n, iso);
		n->type = FUNC;
		n->scope = NULL;
		return n;
	}

	ASTVar* ASTVar::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTVar));
		ASTVar* n = new(pt) ASTVar();
		ASTNode::SetDefaults(n, iso);
		n->type = VAR;
		return n;
	}

} // namespace internal
}