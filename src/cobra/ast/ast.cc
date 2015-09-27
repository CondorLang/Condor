#include "ast.h"
#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{
	//http://stackoverflow.com/questions/2995099/malloc-and-constructors
	ASTNode* ASTNode::New(Isolate* iso){
		ASTNode* n = (ASTNode*) iso->GetMemory(sizeof(ASTNode));
		ASTNode::SetDefaults(n);
    return n; 
	}

	void ASTNode::SetDefaults(ASTNode* node){
		node->visibility = vPUBLIC;
		node->scan = true;
		node->row = 0;
		node->col = 0;
		node->used = false;
		node->type = ILLEGAL;
	}

	ASTNull* ASTNull::New(Isolate* iso){
		ASTNull* n = (ASTNull*) iso->GetMemory(sizeof(ASTNull));
		ASTNode::SetDefaults(n);
		n->type = EXPR;
		n->value = NULL;
    return n;
	}

	ASTCastExpr* ASTCastExpr::New(Isolate* iso){
		ASTCastExpr* n = (ASTCastExpr*) iso->GetMemory(sizeof(ASTCastExpr));
		ASTNode::SetDefaults(n);
		n->type = ASTCAST_EXPR;
		n->value = NULL;
		n->assignType = kNULL;
    return n;
	}

	ASTIdent* ASTIdent::New(Isolate* iso){
		ASTIdent* n = (ASTIdent*) iso->GetMemory(sizeof(ASTIdent));
		ASTNode::SetDefaults(n);
		n->type = IDENT;
		n->value = NULL;
		n->inc = false;
		n->dec = false;
		n->pre = false;
		n->post = false;
    return n;
	}

	ASTLiterary* ASTLiterary::New(Isolate* iso){
		ASTLiterary* n = (ASTLiterary*) iso->GetMemory(sizeof(ASTLiterary));
		ASTNode::SetDefaults(n);
		n->type = LITERARY;
    return n;
	}

	ASTUnaryExpr* ASTUnaryExpr::New(Isolate* iso){
		ASTUnaryExpr* n = (ASTUnaryExpr*) iso->GetMemory(sizeof(ASTUnaryExpr));
		ASTNode::SetDefaults(n);
		n->type = UNARY;
		n->value = NULL;
		n->op = NULL;
    return n;
	}

	ASTBinaryExpr* ASTBinaryExpr::New(Isolate* iso){
		ASTBinaryExpr* n = (ASTBinaryExpr*) iso->GetMemory(sizeof(ASTBinaryExpr));
		ASTNode::SetDefaults(n);
		n->type = BINARY;
		n->Left = NULL;
		n->Right = NULL;
		n->op = NULL;
    return n;
	}

	ASTArrayMemberExpr* ASTArrayMemberExpr::New(Isolate* iso){
		ASTArrayMemberExpr* n = (ASTArrayMemberExpr*) iso->GetMemory(sizeof(ASTArrayMemberExpr));
		ASTNode::SetDefaults(n);
		n->type = ARRAY_MEMBER;
		n->member = NULL;
		n->value = NULL;
    return n;
	}

	ASTObjectMemberChainExpr* ASTObjectMemberChainExpr::New(Isolate* iso){
		ASTObjectMemberChainExpr* n = (ASTObjectMemberChainExpr*) iso->GetMemory(sizeof(ASTObjectMemberChainExpr));
		ASTNode::SetDefaults(n);
		n->type = OBJECT_MEMBER_CHAIN;
		n->isSetting = false;
		n->member = NULL;
		n->object = NULL;
		n->value = NULL;
    return n;
	}

	ASTVar* ASTVar::New(Isolate* iso){
		ASTVar* n = (ASTVar*) iso->GetMemory(sizeof(ASTVar));
		ASTNode::SetDefaults(n);
		n->type = VAR;
		n->stmt = NULL;
		n->varClass = NULL;
		n->cast = false;
		n->castType = UNDEFINED;
    return n;
	}

	ASTParamVar* ASTParamVar::New(Isolate* iso){
		ASTParamVar* n = (ASTParamVar*) iso->GetMemory(sizeof(ASTParamVar));
		ASTNode::SetDefaults(n);
		n->type = ASTPARAM_VAR;
    return n;
	}

	ASTVarList* ASTVarList::New(Isolate* iso){
		ASTVarList* n = (ASTVarList*) iso->GetMemory(sizeof(ASTVarList));
		ASTNode::SetDefaults(n);
		n->type = VARLIST;
		n->vars.SetIsolate(iso);
    return n;
	}

	ASTInt* ASTInt::New(Isolate* iso){
		ASTInt* n = (ASTInt*) iso->GetMemory(sizeof(ASTInt));
		ASTNode::SetDefaults(n);
		n->type = INT;
		n->value = 0;
    return n;
	}

	ASTFloat* ASTFloat::New(Isolate* iso){
		ASTFloat* n = (ASTFloat*) iso->GetMemory(sizeof(ASTFloat));
		ASTNode::SetDefaults(n);
		n->type = FLOAT;
		n->value = 0.0;
    return n;
	}

	ASTDouble* ASTDouble::New(Isolate* iso){
		ASTDouble* n = (ASTDouble*) iso->GetMemory(sizeof(ASTDouble));
		ASTNode::SetDefaults(n);
		n->type = DOUBLE;
		n->value = 0.0;
    return n;
	}

	ASTBoolean* ASTBoolean::New(Isolate* iso){
		ASTBoolean* n = (ASTBoolean*) iso->GetMemory(sizeof(ASTBoolean));
		ASTNode::SetDefaults(n);
		n->type = BOOLEAN;
		n->value = false;
    return n;
	}

	ASTChar* ASTChar::New(Isolate* iso){
		ASTChar* n = (ASTChar*) iso->GetMemory(sizeof(ASTChar));
		ASTNode::SetDefaults(n);
		n->type = CHAR;
		n->value = '\0';
    return n;
	}

	ASTString* ASTString::New(Isolate* iso){
		ASTString* n = (ASTString*) iso->GetMemory(sizeof(ASTString));
		ASTNode::SetDefaults(n);
		n->type = STRING;
    return n;
	}

	ASTBlock* ASTBlock::New(Isolate* iso){
		ASTBlock* n = (ASTBlock*) iso->GetMemory(sizeof(ASTBlock));
		ASTNode::SetDefaults(n);
		n->type = BLOCK;
		n->scope = NULL;
    return n;
	}

	ASTFunc* ASTFunc::New(Isolate* iso){
		ASTFunc* n = (ASTFunc*) iso->GetMemory(sizeof(ASTFunc));
		ASTNode::SetDefaults(n);
		n->type = FUNC;
		n->body = NULL;
		n->used = false;
		n->returnType = ILLEGAL;
		n->args.SetIsolate(iso);
    return n;
	}

	ASTFuncCallExpr* ASTFuncCallExpr::New(Isolate* iso){
		ASTFuncCallExpr* n = (ASTFuncCallExpr*) iso->GetMemory(sizeof(ASTFuncCallExpr));
		ASTNode::SetDefaults(n);
		n->type = FUNC_CALL;
		n->isNew = false;
		n->pos = 0;
		n->func = NULL;
		n->scope = NULL;
		n->assignType = ILLEGAL;
		n->params.SetIsolate(iso);
    return n;
	}

	ASTObjectInit* ASTObjectInit::New(Isolate* iso){
		ASTObjectInit* n = (ASTObjectInit*) iso->GetMemory(sizeof(ASTObjectInit));
		ASTNode::SetDefaults(n);
		n->params.SetIsolate(iso);
		n->type = OBJECT_INIT;
		n->isNew = false;
		n->pos = 0;
		n->func = NULL;
		n->scope = NULL;
		n->assignType = ILLEGAL;
    return n;
	}

	ASTArray* ASTArray::New(Isolate* iso, TOKEN rType){
		ASTArray* n = (ASTArray*) iso->GetMemory(sizeof(ASTArray));
		ASTNode::SetDefaults(n);
		n->type = ARRAY;
		n->arrayType = rType;
		n->value.SetIsolate(iso);
    return n;
	}

	ASTObject* ASTObject::New(Isolate* iso){
		ASTObject* n = (ASTObject*) iso->GetMemory(sizeof(ASTObject));
		ASTNode::SetDefaults(n);
		n->type = OBJECT;
		n->members.SetIsolate(iso);
    return n;
	}

	ASTIf* ASTIf::New(Isolate* iso){
		ASTIf* n = (ASTIf*) iso->GetMemory(sizeof(ASTIf));
		ASTNode::SetDefaults(n);
		n->type = IF;
		n->conditions = NULL;
		n->block = NULL;
    return n;
	}

	ASTElse* ASTElse::New(Isolate* iso){
		ASTElse* n = (ASTElse*) iso->GetMemory(sizeof(ASTElse));
		ASTNode::SetDefaults(n);
		n->type = ELSE;
		n->ifStmt = NULL;
		n->conditions = NULL;
		n->block = NULL;
		n->ifStmt = NULL;
    return n;
	}

	ASTWhile* ASTWhile::New(Isolate* iso){
		ASTWhile* n = (ASTWhile*) iso->GetMemory(sizeof(ASTWhile));
		ASTNode::SetDefaults(n);
		n->type = WHILE;
		n->conditions = NULL;
		n->block = NULL;
    return n;
	}

	ASTFor* ASTFor::New(Isolate* iso){
		ASTFor* n = (ASTFor*) iso->GetMemory(sizeof(ASTFor));
		ASTNode::SetDefaults(n);
		n->type = FOR;
		n->var = NULL;
		n->conditions = NULL;
		n->iterator = NULL;
		n->block = NULL;
    return n;
	}

	ASTTryCatch* ASTTryCatch::New(Isolate* iso){
		ASTTryCatch* n = (ASTTryCatch*) iso->GetMemory(sizeof(ASTTryCatch));
		ASTNode::SetDefaults(n);
		n->type = TRY_CATCH;
		n->tryBlock = NULL;
		n->catchFunc = NULL;
    return n;
	}

	ASTThrow* ASTThrow::New(Isolate* iso){
		ASTThrow* n = (ASTThrow*) iso->GetMemory(sizeof(ASTThrow));
		ASTNode::SetDefaults(n);
		n->type = THROW;
		n->obj = NULL;
    return n;
	}

	ASTImport* ASTImport::New(Isolate* iso){
		ASTImport* n = (ASTImport*) iso->GetMemory(sizeof(ASTImport));
		ASTNode::SetDefaults(n);
		n->type = IMPORT;
    return n;
	}

	ASTInclude* ASTInclude::New(Isolate* iso){
		ASTInclude* n = (ASTInclude*) iso->GetMemory(sizeof(ASTInclude));
		ASTNode::SetDefaults(n);
		n->type = INCLUDE;
    return n;
	}

	ASTFile* ASTFile::New(Isolate* iso){
		ASTFile* n = (ASTFile*) iso->GetMemory(sizeof(ASTFile));
		ASTNode::SetDefaults(n);
		n->type = FILE;
		n->scope = NULL;
		n->includesList.SetIsolate(iso);
    return n;
	}

} // namespace internal
}