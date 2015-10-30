#include "ast.h"
#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{
	//http://stackoverflow.com/questions/2995099/malloc-and-constructors
	ASTNode* ASTNode::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTNode));
		ASTNode* n = new(p) ASTNode();
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

	ASTUndefined* ASTUndefined::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTUndefined));
		ASTUndefined* n = new(p) ASTUndefined();
		ASTNode::SetDefaults(n);
		n->type = UNDEFINED;
		n->name = "undefined";
		n->assignType = UNDEFINED;
		return n;
	}

	ASTExpr* ASTExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTExpr));
		ASTExpr* n = new(p) ASTExpr();
		ASTNode::SetDefaults(n);
		n->type = EXPR;
		n->value = NULL;
		return n;
	}

	ASTNot* ASTNot::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTNot));
		ASTNot* n = new(p) ASTNot();
		ASTNode::SetDefaults(n);
		n->type = AST_NOT;
		n->value = NULL;
		n->assignType = BOOLEAN;
		return n;
	}

	ASTNull* ASTNull::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTNull));
		ASTNull* n = new(p) ASTNull();
		ASTNode::SetDefaults(n);
		n->type = ASTNULL;
		n->value = NULL;
    return n;
	}

	ASTCastExpr* ASTCastExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTCastExpr));
		ASTCastExpr* n = new(p) ASTCastExpr();
		ASTNode::SetDefaults(n);
		n->type = ASTCAST_EXPR;
		n->value = NULL;
		n->assignType = kNULL;
    return n;
	}

	ASTIdent* ASTIdent::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTIdent));
		ASTIdent* n = new(p) ASTIdent();
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
		void* p = iso->GetMemory(sizeof(ASTLiterary));
		ASTLiterary* n = new(p) ASTLiterary();
		ASTNode::SetDefaults(n);
		n->type = LITERARY;
    return n;
	}

	ASTUnaryExpr* ASTUnaryExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTUnaryExpr));
		ASTUnaryExpr* n = new(p) ASTUnaryExpr();
		ASTNode::SetDefaults(n);
		n->type = UNARY;
		n->value = NULL;
		n->op = NULL;
    return n;
	}

	ASTBinaryExpr* ASTBinaryExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTBinaryExpr));
		ASTBinaryExpr* n = new(p) ASTBinaryExpr();
		ASTNode::SetDefaults(n);
		n->type = BINARY;
		n->Left = NULL;
		n->Right = NULL;
		n->op = NULL;
    return n;
	}

	ASTArrayMemberExpr* ASTArrayMemberExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTArrayMemberExpr));
		ASTArrayMemberExpr* n = new(p) ASTArrayMemberExpr();
		ASTNode::SetDefaults(n);
		n->type = ARRAY_MEMBER;
		n->member = NULL;
		n->value = NULL;
    return n;
	}

	ASTObjectMemberChainExpr* ASTObjectMemberChainExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTObjectMemberChainExpr));
		ASTObjectMemberChainExpr* n = new(p) ASTObjectMemberChainExpr();
		ASTNode::SetDefaults(n);
		n->type = OBJECT_MEMBER_CHAIN;
		n->isSetting = false;
		n->member = NULL;
		n->object = NULL;
		n->value = NULL;
    return n;
	}

	ASTVar* ASTVar::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTVar));
		ASTVar* n = new(p) ASTVar();
		ASTNode::SetDefaults(n);
		n->type = VAR;
		n->stmt = NULL;
		n->varClass = NULL;
		n->cast = false;
		n->castType = UNDEFINED;
		n->array = false;
    return n;
	}

	ASTVarList* ASTVarList::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTVarList));
		ASTVarList* n = new(p) ASTVarList();
		ASTNode::SetDefaults(n);
		n->type = VARLIST;
    return n;
	}

	ASTInt* ASTInt::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTInt));
		ASTInt* n = new(p) ASTInt();
		ASTNode::SetDefaults(n);
		n->type = INT;
		n->value = 0;
    return n;
	}

	ASTFloat* ASTFloat::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTFloat));
		ASTFloat* n = new(p) ASTFloat();
		ASTNode::SetDefaults(n);
		n->type = FLOAT;
		n->value = 0.0;
    return n;
	}

	ASTDouble* ASTDouble::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTDouble));
		ASTDouble* n = new(p) ASTDouble();
		ASTNode::SetDefaults(n);
		n->type = DOUBLE;
		n->value = 0.0;
    return n;
	}

	ASTBoolean* ASTBoolean::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTBoolean));
		ASTBoolean* n = new(p) ASTBoolean();
		ASTNode::SetDefaults(n);
		n->type = BOOLEAN;
		n->value = false;
    return n;
	}

	ASTChar* ASTChar::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTChar));
		ASTChar* n = new(p) ASTChar();
		ASTNode::SetDefaults(n);
		n->type = CHAR;
		n->value = '\0';
    return n;
	}

	ASTString* ASTString::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTString));
		ASTString* n = new(p) ASTString();
		ASTNode::SetDefaults(n);
		n->type = STRING;
    return n;
	}

	ASTBlock* ASTBlock::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTBlock));
		ASTBlock* n = new(p) ASTBlock();
		ASTNode::SetDefaults(n);
		n->type = BLOCK;
		n->scope = NULL;
    return n;
	}

	ASTFunc* ASTFunc::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTFunc));
		ASTFunc* n = new(p) ASTFunc();
		ASTNode::SetDefaults(n);
		n->type = FUNC;
		n->body = NULL;
		n->used = false;
		n->returnType = ILLEGAL;
    return n;
	}

	ASTFuncCallExpr* ASTFuncCallExpr::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTFuncCallExpr));
		ASTFuncCallExpr* n = new(p) ASTFuncCallExpr();
		ASTNode::SetDefaults(n);
		n->type = FUNC_CALL;
		n->isNew = false;
		n->pos = 0;
		n->func = NULL;
		n->scope = NULL;
		n->assignType = ILLEGAL;
    return n;
	}

	ASTObjectInit* ASTObjectInit::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTObjectInit));
		ASTObjectInit* n = new(p) ASTObjectInit();
		ASTNode::SetDefaults(n);
		n->type = OBJECT_INIT;
		n->isNew = false;
		n->pos = 0;
		n->func = NULL;
		n->scope = NULL;
		n->assignType = ILLEGAL;
		n->base = NULL;
    return n;
	}

	ASTArray* ASTArray::New(Isolate* iso, TOKEN rType){
		void* p = iso->GetMemory(sizeof(ASTArray));
		ASTArray* n = new(p) ASTArray();
		ASTNode::SetDefaults(n);
		n->type = ARRAY;
		n->arrayType = rType;
		n->varClass = NULL;
    return n;
	}

	ASTObject* ASTObject::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTObject));
		ASTObject* n = new(p) ASTObject();
		ASTNode::SetDefaults(n);
		n->type = OBJECT;
    return n;
	}

	ASTIf* ASTIf::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTIf));
		ASTIf* n = new(p) ASTIf();
		ASTNode::SetDefaults(n);
		n->type = IF;
		n->conditions = NULL;
		n->block = NULL;
    return n;
	}

	ASTElse* ASTElse::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTElse));
		ASTElse* n = new(p) ASTElse();
		ASTNode::SetDefaults(n);
		n->type = ELSE;
		n->ifStmt = NULL;
		n->conditions = NULL;
		n->block = NULL;
		n->ifStmt = NULL;
    return n;
	}

	ASTWhile* ASTWhile::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTWhile));
		ASTWhile* n = new(p) ASTWhile();
		ASTNode::SetDefaults(n);
		n->type = WHILE;
		n->conditions = NULL;
		n->block = NULL;
    return n;
	}

	ASTFor* ASTFor::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTFor));
		ASTFor* n = new(p) ASTFor();
		ASTNode::SetDefaults(n);
		n->type = FOR;
		n->var = NULL;
		n->conditions = NULL;
		n->iterator = NULL;
		n->block = NULL;
    return n;
	}

	ASTTryCatch* ASTTryCatch::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTTryCatch));
		ASTTryCatch* n = new(p) ASTTryCatch();
		ASTNode::SetDefaults(n);
		n->type = TRY_CATCH;
		n->tryBlock = NULL;
		n->catchFunc = NULL;
    return n;
	}

	ASTThrow* ASTThrow::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTThrow));
		ASTThrow* n = new(p) ASTThrow();
		ASTNode::SetDefaults(n);
		n->type = THROW;
		n->obj = NULL;
    return n;
	}

	ASTImport* ASTImport::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTImport));
		ASTImport* n = new(p) ASTImport();
		ASTNode::SetDefaults(n);
		n->type = IMPORT;
    return n;
	}

	ASTInclude* ASTInclude::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTInclude));
		ASTInclude* n = new(p) ASTInclude();
		ASTNode::SetDefaults(n);
		n->type = INCLUDE;
    return n;
	}

	ASTFile* ASTFile::New(Isolate* iso){
		void* p = iso->GetMemory(sizeof(ASTFile));
		ASTFile* n = new(p) ASTFile();
		ASTNode::SetDefaults(n);
		n->type = FILE;
		n->scope = NULL;
    return n;
	}

} // namespace internal
}