// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The end user will not interact with this library.
 * This file is the basic structures for AST.
 *
 * User:
 * 	
 * 
 * Usage:	
 */

#ifndef AST_H_
#define AST_H_

#include "condor/token/token.h"

typedef struct ASTMeta{
	int id;
} ASTMeta;

typedef struct ASTLiteral{
	char* value;
	Token dataType;
	ASTMeta meta;
} ASTLiteral;

typedef struct ASTVar{
	char* name;
	void* value;
	Token dataType;
	ASTMeta meta; // meta information on every node
} ASTVar;

typedef struct ASTBinaryExpr{
	Token op; // operator
	ASTMeta meta;
} ASTBinaryExpr;

void InitVars(ASTVar vars[], int len);
void InitBinaryExprs(ASTBinaryExpr exprs[], int len);
void InitLiterals(ASTLiteral lits[], int len);

#endif // AST_H_
