#include "sizes.h"

namespace Cobra {
namespace internal{
namespace Sizes{

	const int GetSize(TOKEN type){
			int t = (int) type;
			switch (t){
				case ILLEGAL: return kAstNode;
				case EXPR: return kAstExpr;
				case ASTNULL: return kAstNull;
				case ASTCAST_EXPR: return kAstCastExpr;
				case IDENT: return kASTIdent;
				case LITERARY: return kASTLiterary;
				case UNARY: return kASTUnaryExpr;
				case BINARY: return kASTBinaryExpr;
				case ARRAY_MEMBER: return kASTArrayMemberExpr;
				case OBJECT_MEMBER_CHAIN: return kASTObjectMemberChainExpr;
				case VAR: return kASTVar;
				case VARLIST: return kASTVarList;
				case INT: return kASTInt;
				case FLOAT: return kASTFloat;
				case DOUBLE: return kASTDouble;
				case BOOLEAN: return kASTBoolean;
				case CHAR: return kASTChar;
				case STRING: return kASTString;
				case BLOCK: return kASTBlock;
				case FUNC: return kASTFunc;
				case FUNC_CALL: return kASTFuncCallExpr;
				case OBJECT_INIT: return kASTObjectInit;
				case ARRAY: return kASTArray;
				case OBJECT: return kASTObject;
				case IF: return kASTIf;
				case ELSE: return kASTElse;
				case WHILE: return kASTWhile;
				case FOR: return kASTFor;
				case TRY_CATCH: return kASTTryCatch;
				case THROW: return kASTThrow;
				case IMPORT: return kASTImport;
				case INCLUDE: return kASTInclude;
				case FILE: return kASTFile;
				default: {
					return kAstNode;
				}
			}
		}

} // namespace Sizes
} // namespace internal
} // namespace Cobra