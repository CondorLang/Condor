// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The end user will not interact with this library.
 * This file we will check the syntax of the inputted 
 * code. We will also build a lightweight AST.
 *
 * User:
 * 	Syntax Analysis Only
 * 
 * Usage:
 * 	char* code = "var a = 10;";
 * 	Lexer* lexer = InitLexer(code);
 * 	Token token = GetNextToken(lexer); 	
 */

#ifndef SYNTAX_H_
#define SYNTAX_H_

#include <stdio.h>
#include <string.h>

#include "utils/assert.h"
#include "condor/token/token.h"
#include "condor/lexer/lexer.h"
#include "condor/mem/allocate.h"
#include "condor/ast/scope.h"
#include "condor/ast/ast.h"
#include "condor/ast/astlist.h"
#include "condor/number/number.h"
#include "utils/debug.h"

typedef struct Syntax {

} Syntax;

ASTNode* GetNextNode(Scope* scope);
ASTList* GetNextASTList(Scope* scope);
ASTListItem* GetNextASTListItem(Scope* scope);

ASTNode* ParseVar(Scope* scope, Lexer* lexer, Token dataType);
ASTNode* ParseExpression(Scope* scope, Lexer* lexer);
ASTNode* ParseFor(Scope* scope, Lexer* lexer);
ASTNode* ParseIf(Scope* scope, Lexer* lexer);
ASTNode* ParseWhile(Scope* scope, Lexer* lexer);
ASTNode* ParseSwitch(Scope* scope, Lexer* lexer);
ASTNode* ParseCase(Scope* scope, Lexer* lexer);
ASTNode* ParseReturn(Scope* scope, Lexer* lexer);
ASTNode* ParseBreak(Scope* scope, Lexer* lexer);
ASTNode* ParseFunc(Scope* scope, Lexer* lexer);
ASTNode* ParseFuncCall(Scope* scope, Lexer* lexer);
ASTList* ParseParams(Scope* scope, Lexer* lexer);
ASTNode* ParseIdent(Scope* scope, Lexer* lexer);
int ParseBody(Scope* scope, Lexer* lexer);
Token ParseStmtList(Scope* scope, Lexer* lexer, int scopeId, bool oneStmt);

#endif // SYNTAX_H_
