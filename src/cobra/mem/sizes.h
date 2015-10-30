#ifndef SIZES_H_
#define SIZES_H_

#include "cobra/ast/ast.h"
// #include "cobra/ast/check.h"
// #include "cobra/ast/scope.h"
// #include "cobra/ast/context.h"
// #include "cobra/mem/factory.h"
// #include "cobra/parser/parser.h"
// #include "cobra/scanner/scanner.h"
#include "cobra/token/token.h"
// #include "cobra/types/try_catch/exception.h"
// #include "cobra/types/script/script.h"
// #include "cobra/types/strings/string.h"

namespace Cobra {
namespace internal{

	namespace Sizes {

		const int kAstNode = sizeof(ASTNode);
		const int kAstExpr = sizeof(ASTExpr);
		const int kAstNull = sizeof(ASTNull);
		const int kAstCastExpr = sizeof(ASTCastExpr);
		const int kASTIdent = sizeof(ASTIdent);
		const int kASTLiterary = sizeof(ASTLiterary);
		const int kASTUnaryExpr = sizeof(ASTUnaryExpr);
		const int kASTBinaryExpr = sizeof(ASTBinaryExpr);
		const int kASTArrayMemberExpr = sizeof(ASTArrayMemberExpr);
		const int kASTObjectMemberChainExpr = sizeof(ASTObjectMemberChainExpr);
		const int kASTVar = sizeof(ASTVar);
		const int kASTVarList = sizeof(ASTVarList);
		const int kASTInt = sizeof(ASTInt);
		const int kASTFloat = sizeof(ASTFloat);
		const int kASTDouble = sizeof(ASTDouble);
		const int kASTBoolean = sizeof(ASTBoolean);
		const int kASTChar = sizeof(ASTChar);
		const int kASTString = sizeof(ASTString);
		const int kASTBlock = sizeof(ASTBlock);
		const int kASTFunc = sizeof(ASTFunc);
		const int kASTFuncCallExpr = sizeof(ASTFuncCallExpr);
		const int kASTObjectInit = sizeof(ASTObjectInit);
		const int kASTArray = sizeof(ASTArray);
		const int kASTObject = sizeof(ASTObject);
		const int kASTIf = sizeof(ASTIf);
		const int kASTElse = sizeof(ASTElse);
		const int kASTWhile = sizeof(ASTWhile);
		const int kASTFor = sizeof(ASTFor);
		const int kASTTryCatch = sizeof(ASTTryCatch);
		const int kASTThrow = sizeof(ASTThrow);
		const int kASTImport = sizeof(ASTImport);
		const int kASTInclude = sizeof(ASTInclude);
		const int kASTFile = sizeof(ASTFile);
		const int GetSize(TOKEN type);
		// const int kContext = sizeof(Context);
		// const int kScope = sizeof(Scope);
		// const int kScanner = sizeof(Scanner);
		// const int kToken = sizeof(Token);
	}

} // namespace internal
} // namespace Cobra

#endif // SIZES_H_