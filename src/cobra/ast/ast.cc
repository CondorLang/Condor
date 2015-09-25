#include "ast.h"
#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{
	//http://stackoverflow.com/questions/2995099/malloc-and-constructors
	ASTNode* ASTNode::New(Isolate* iso){
		ASTNode* n = (ASTNode*) iso->GetMemory(sizeof(ASTNode));
		n->type = ILLEGAL;
		n->visibility = vPUBLIC;
		n->scan = true;
		n->row = 0;
		n->col = 0;
		n->used = false;
		return n; 
	}

	ASTNull* ASTNull::New(Isolate* iso){
		ASTNull* n = (ASTNull*) iso->GetMemory(sizeof(ASTNull));
		n->type = EXPR;
		n->value = NULL;
		return n;
	}

	ASTCastExpr* ASTCastExpr::New(Isolate* iso){
		ASTCastExpr* n = (ASTCastExpr*) iso->GetMemory(sizeof(ASTCastExpr));
		n->type = kNULL;
		n->value = NULL;
		n->assignType = kNULL;
		return n;
	}

	ASTIdent* ASTIdent::New(Isolate* iso){
		ASTIdent* n = (ASTIdent*) iso->GetMemory(sizeof(ASTIdent));
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
		n->type = LITERARY;
		return n;
	}

	ASTUnaryExpr* ASTUnaryExpr::New(Isolate* iso){
		ASTUnaryExpr* n = (ASTUnaryExpr*) iso->GetMemory(sizeof(ASTUnaryExpr));
		n->type = UNARY;
		n->value = NULL;
		n->op = NULL;
		return n;
	}

	ASTBinaryExpr* ASTBinaryExpr::New(Isolate* iso){
		ASTBinaryExpr* n = (ASTBinaryExpr*) iso->GetMemory(sizeof(ASTBinaryExpr));
		n->type = BINARY;
		n->Left = NULL;
		n->Right = NULL;
		n->op = NULL;
		return n;
	}

	ASTArrayMemberExpr* ASTArrayMemberExpr::New(Isolate* iso){
		ASTArrayMemberExpr* n = (ASTArrayMemberExpr*) iso->GetMemory(sizeof(ASTArrayMemberExpr));
		n->type = ARRAY_MEMBER;
		n->member = NULL;
		n->value = NULL;
		return n;
	}

	ASTObjectMemberChainExpr* ASTObjectMemberChainExpr::New(Isolate* iso){
		ASTObjectMemberChainExpr* n = (ASTObjectMemberChainExpr*) iso->GetMemory(sizeof(ASTObjectMemberChainExpr));
		n->type = OBJECT_MEMBER_CHAIN;
		n->isSetting = false;
		n->member = NULL;
		n->object = NULL;
		n->value = NULL;
		return n;
	}

	ASTVar* ASTVar::New(Isolate* iso){
		ASTVar* n = (ASTVar*) iso->GetMemory(sizeof(ASTVar));
		n->type = VAR;
		n->stmt = NULL;
		n->varClass = NULL;
		n->cast = false;
		n->castType = UNDEFINED;
		return n;
	}

	ASTParamVar* ASTParamVar::New(Isolate* iso){
		ASTParamVar* n = (ASTParamVar*) iso->GetMemory(sizeof(ASTParamVar));
		n->type = ASTPARAM_VAR;
		return n;
	}

	ASTVarList* ASTVarList::New(Isolate* iso){
		ASTVarList* n = (ASTVarList*) iso->GetMemory(sizeof(ASTVarList));
		n->type = VARLIST;
		n->vars.SetIsolate(iso);
		return n;
	}

	ASTInt* ASTInt::New(Isolate* iso){
		ASTInt* n = (ASTInt*) iso->GetMemory(sizeof(ASTInt));
		n->type = INT;
		n->value = 0;
		return n;
	}

	ASTFloat* ASTFloat::New(Isolate* iso){
		ASTFloat* n = (ASTFloat*) iso->GetMemory(sizeof(ASTFloat));
		n->type = FLOAT;
		n->value = 0.0;
		return n;
	}

	ASTDouble* ASTDouble::New(Isolate* iso){
		ASTDouble* n = (ASTDouble*) iso->GetMemory(sizeof(ASTDouble));
		n->type = DOUBLE;
		n->value = 0.0;
		return n;
	}

	ASTBoolean* ASTBoolean::New(Isolate* iso){
		ASTBoolean* n = (ASTBoolean*) iso->GetMemory(sizeof(ASTBoolean));
		n->type = BOOLEAN;
		n->value = false;
		return n;
	}

	ASTChar* ASTChar::New(Isolate* iso){
		ASTChar* n = (ASTChar*) iso->GetMemory(sizeof(ASTChar));
		n->type = CHAR;
		n->value = '\0';
		return n;
	}

	ASTString* ASTString::New(Isolate* iso){
		ASTString* n = (ASTString*) iso->GetMemory(sizeof(ASTString));
		n->type = STRING;
		return n;
	}

	ASTBlock* ASTBlock::New(Isolate* iso){
		ASTBlock* n = (ASTBlock*) iso->GetMemory(sizeof(ASTBlock));
		n->type = BLOCK;
		n->scope = NULL;
		return n;
	}

	ASTFunc* ASTFunc::New(Isolate* iso){
		ASTFunc* n = (ASTFunc*) iso->GetMemory(sizeof(ASTFunc));
		n->type = FUNC;
		n->body = NULL;
		n->used = false;
		n->returnType = ILLEGAL;
		n->args.SetIsolate(iso);
		return n;
	}

	ASTFuncCallExpr* ASTFuncCallExpr::New(Isolate* iso){
		ASTFuncCallExpr* n = (ASTFuncCallExpr*) iso->GetMemory(sizeof(ASTFuncCallExpr));
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
		n->type = OBJECT_INIT;
		return n;
	}

	ASTArray* ASTArray::New(Isolate* iso, TOKEN rType){
		ASTArray* n = (ASTArray*) iso->GetMemory(sizeof(ASTArray));
		n->type = ARRAY;
		n->arrayType = rType;
		n->value.SetIsolate(iso);
		return n;
	}

	ASTObject* ASTObject::New(Isolate* iso){
		ASTObject* n = (ASTObject*) iso->GetMemory(sizeof(ASTObject));
		n->type = OBJECT;
		n->members.SetIsolate(iso);
		return n;
	}

	ASTIf* ASTIf::New(Isolate* iso){
		ASTIf* n = (ASTIf*) iso->GetMemory(sizeof(ASTIf));
		n->type = IF;
		n->conditions = NULL;
		n->block = NULL;
		return n;
	}

	ASTElse* ASTElse::New(Isolate* iso){
		ASTElse* n = (ASTElse*) iso->GetMemory(sizeof(ASTElse));
		n->type = ELSE;
		n->ifStmt = NULL;
		n->conditions = NULL;
		n->block = NULL;
		n->ifStmt = NULL;
		return n;
	}

	ASTWhile* ASTWhile::New(Isolate* iso){
		ASTWhile* n = (ASTWhile*) iso->GetMemory(sizeof(ASTWhile));
		n->type = WHILE;
		n->conditions = NULL;
		n->block = NULL;
		return n;
	}

	ASTFor* ASTFor::New(Isolate* iso){
		ASTFor* n = (ASTFor*) iso->GetMemory(sizeof(ASTFor));
		n->type = FOR;
		n->var = NULL;
		n->conditions = NULL;
		n->iterator = NULL;
		n->block = NULL;
		return n;
	}

	ASTTryCatch* ASTTryCatch::New(Isolate* iso){
		ASTTryCatch* n = (ASTTryCatch*) iso->GetMemory(sizeof(ASTTryCatch));
		n->type = TRY_CATCH;
		n->tryBlock = NULL;
		n->catchFunc = NULL;
		return n;
	}

	ASTThrow* ASTThrow::New(Isolate* iso){
		ASTThrow* n = (ASTThrow*) iso->GetMemory(sizeof(ASTThrow));
		n->type = THROW;
		n->obj = NULL;
		return n;
	}

	ASTImport* ASTImport::New(Isolate* iso){
		ASTImport* n = (ASTImport*) iso->GetMemory(sizeof(ASTImport));
		n->type = IMPORT;
		return n;
	}

	ASTInclude* ASTInclude::New(Isolate* iso){
		ASTInclude* n = (ASTInclude*) iso->GetMemory(sizeof(ASTInclude));
		n->type = INCLUDE;
		return n;
	}

	ASTFile* ASTFile::New(Isolate* iso){
		ASTFile* n = (ASTFile*) iso->GetMemory(sizeof(ASTFile));
		n->type = FILE;
		n->scope = NULL;
		n->includesList.SetIsolate(iso);
		return n;
	}

} // namespace internal
}