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
		node->isExport = false;
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
		n->baseName = "";
		n->baseType = UNDEFINED;
		n->value = NULL;
		n->assignmentType = ILLEGAL;
		return n;
	}

	ASTExpr* ASTExpr::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTExpr));
		ASTExpr* n = new(pt) ASTExpr();
		ASTNode::SetDefaults(n, iso);
		n->type = EXPR;
		return n;
	}

	ASTBinaryExpr* ASTBinaryExpr::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTBinaryExpr));
		ASTBinaryExpr* n = new(pt) ASTBinaryExpr();
		ASTNode::SetDefaults(n, iso);
		n->type = BINARY;
		n->left = NULL;
		n->right = NULL;
		n->isBoolean = false;
		n->op = ILLEGAL;
		return n;
	}

	ASTLiteral* ASTLiteral::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTLiteral));
		ASTLiteral* n = new(pt) ASTLiteral();
		ASTNode::SetDefaults(n, iso);
		n->value = "";
		n->type = LITERAL;
		n->litType = UNDEFINED;
		n->unary = UNDEFINED;
		n->isPost = true;
		return n;
	}

	ASTFuncCall* ASTFuncCall::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTFuncCall));
		ASTFuncCall* n = new(pt) ASTFuncCall();
		ASTNode::SetDefaults(n, iso);
		n->type = FUNC_CALL;
		return n;
	}

	ASTForExpr* ASTForExpr::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTForExpr));
		ASTForExpr* n = new(pt) ASTForExpr();
		ASTNode::SetDefaults(n, iso);
		n->type = FOR;
		n->tick = NULL;
		n->condition = NULL;
		n->var = NULL;
		n->scope = NULL;
		return n;
	}

	ASTWhileExpr* ASTWhileExpr::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTWhileExpr));
		ASTWhileExpr* n = new(pt) ASTWhileExpr();
		ASTNode::SetDefaults(n, iso);
		n->type = WHILE;
		n->condition = NULL;
		return n;
	}

	ASTTryCatchExpr* ASTTryCatchExpr::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTTryCatchExpr));
		ASTTryCatchExpr* n = new(pt) ASTTryCatchExpr();
		ASTNode::SetDefaults(n, iso);
		n->type = TRY_CATCH;
		n->tryScope = NULL;
		n->catchScope = NULL;
		return n;
	}

	ASTThrow* ASTThrow::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTThrow));
		ASTThrow* n = new(pt) ASTThrow();
		ASTNode::SetDefaults(n, iso);
		n->type = THROW;
		n->expr = NULL;
		return n;
	}

	ASTIf* ASTIf::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTIf));
		ASTIf* n = new(pt) ASTIf();
		ASTNode::SetDefaults(n, iso);
		n->type = IF;
		n->condition = NULL;
		n->scope = NULL;
		return n;
	}

} // namespace internal
}