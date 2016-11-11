// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The end user will not interact with this library.
 * This file is the basic structures for Scope object.
 *
 * User:
 * 	Syntax Analysis Only
 * 
 * Usage: 	
 */

#ifndef SCOPE_H_
#define SCOPE_H_

#include "ast.h"
#include "condor/mem/allocate.h"
#include "condor/lexer/lexer.h"

/**
 * A scope can be defined as a statment body. They include:
 *  - The global scope
 *  - Function bodies
 *  - For, while, do/while bodies
 *  - If else if bodies
 */
typedef struct Scope{
	ASTNodeMemoryAudit* audit;
	int varSpot;
	int binarySpot;
	ASTVar* vars;
	ASTBinaryExpr* binarys;
	ASTLiteral* lits;
} Scope;

void DestroyScope(Scope* scope);
void InitScope(Scope* scope);

#endif // SCOPE_H_
