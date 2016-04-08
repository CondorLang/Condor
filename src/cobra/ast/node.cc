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
		node->isInParen = false;
		node->id = iso->GetContext()->GetNextAstId();
	}

	bool ASTNode::HasVisibility(TOKEN tok){
		for (int i = 0; i < visibility.size(); i++){
			if (visibility[i] == tok) return true;
		}
		return visibility.size() == 0 && tok == PUBLIC;
	}

	void ASTNode::Free(Isolate* iso){
		int t = (int) type;
		switch (t){
			case IMPORT: iso->FreeMemory(this, sizeof(ASTImport)); break;
			case INCLUDE: iso->FreeMemory(this, sizeof(ASTInclude)); break;
			case FUNC: iso->FreeMemory(this, sizeof(ASTFunc)); break;
			case VAR: iso->FreeMemory(this, sizeof(ASTVar)); break;
			case EXPR: iso->FreeMemory(this, sizeof(ASTExpr)); break;
			case BINARY: iso->FreeMemory(this, sizeof(ASTBinaryExpr)); break;
			case LITERAL: iso->FreeMemory(this, sizeof(ASTLiteral)); break;
			case FUNC_CALL: iso->FreeMemory(this, sizeof(ASTFuncCall)); break;
			case FOR: iso->FreeMemory(this, sizeof(ASTForExpr)); break;
			case WHILE: iso->FreeMemory(this, sizeof(ASTWhileExpr)); break;
			case TRY_CATCH: iso->FreeMemory(this, sizeof(ASTTryCatchExpr)); break;
			case THROW: iso->FreeMemory(this, sizeof(ASTThrow)); break;
			case IF: iso->FreeMemory(this, sizeof(ASTIf)); break;
			case DELETE: iso->FreeMemory(this, sizeof(ASTDelete)); break;
			case CASE: iso->FreeMemory(this, sizeof(ASTCase)); break;
			case SWITCH: iso->FreeMemory(this, sizeof(ASTSwitch)); break;
			case OBJECT: iso->FreeMemory(this, sizeof(ASTObject)); break;
			case UNDEFINED: iso->FreeMemory(this, sizeof(ASTUndefined)); break;
			case ARRAY: iso->FreeMemory(this, sizeof(ASTArray)); break;
			case OBJECT_INSTANCE: iso->FreeMemory(this, sizeof(ASTObjectInstance)); break;
		}
	}

	ASTToken* ASTToken::New(Isolate* iso, TOKEN tok){
		void* pt = iso->GetMemory(sizeof(ASTToken));
		ASTToken* n = new(pt) ASTToken();
		ASTNode::SetDefaults(n, iso);
		n->type = TOK;
		n->value = Token::New(iso, tok);
		return n;
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
		n->assignmentType = UNDEFINED;
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
		n->isArray = false;
		n->isObject = false;
		n->member = NULL;
		n->isArg = false;
		n->order = -1;
		return n;
	}

	ASTExpr* ASTExpr::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTExpr));
		ASTExpr* n = new(pt) ASTExpr();
		ASTNode::SetDefaults(n, iso);
		n->type = EXPR;
		n->cast = NULL;
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
		n->isChain = false;
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
		n->var = NULL;
		n->isCast = false;
		return n;
	}

	ASTFuncCall* ASTFuncCall::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTFuncCall));
		ASTFuncCall* n = new(pt) ASTFuncCall();
		ASTNode::SetDefaults(n, iso);
		n->type = FUNC_CALL;
		n->isInternal = false;
		n->func = NULL;
		n->isInit = false;
		n->callback = nullptr;
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

	ASTDelete* ASTDelete::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTDelete));
		ASTDelete* n = new(pt) ASTDelete();
		ASTNode::SetDefaults(n, iso);
		n->type = DELETE;
		n->node = NULL;
		return n;
	}

	ASTCase* ASTCase::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTCase));
		ASTCase* n = new(pt) ASTCase();
		ASTNode::SetDefaults(n, iso);
		n->type = CASE;
		n->condition = NULL;
		n->scope = NULL;
		n->isDefault = false;
		return n;
	}

	ASTSwitch* ASTSwitch::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTSwitch));
		ASTSwitch* n = new(pt) ASTSwitch();
		ASTNode::SetDefaults(n, iso);
		n->type = SWITCH;
		n->value = NULL;
		return n;
	}

	ASTObject* ASTObject::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTObject));
		ASTObject* n = new(pt) ASTObject();
		ASTNode::SetDefaults(n, iso);
		n->type = OBJECT;
		n->scope = NULL;
		n->extend = false;
		return n;
	}

	ASTUndefined* ASTUndefined::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTUndefined));
		ASTUndefined* n = new(pt) ASTUndefined();
		ASTNode::SetDefaults(n, iso);
		n->type = UNDEFINED;
		return n;
	}

	ASTArray* ASTArray::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTArray));
		ASTArray* n = new(pt) ASTArray();
		ASTNode::SetDefaults(n, iso);
		n->type = ARRAY;
		return n;
	}

	ASTObjectInstance* ASTObjectInstance::New(Isolate* iso){
		void* pt = iso->GetMemory(sizeof(ASTObjectInstance));
		ASTObjectInstance* n = new(pt) ASTObjectInstance();
		ASTNode::SetDefaults(n, iso);
		n->type = OBJECT_INSTANCE;
		n->constructor = NULL;
		n->base = NULL;
		return n;
	}

} // namespace internal
}