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

typedef struct ASTNode ASTNode; // forward declare

/**
 * A scope can be defined as a statment body. They include:
 *  - The global scope
 *  - Function bodies
 *  - For, while, do/while bodies
 *  - If else if bodies
 *
 * All variables and ASTNodes are allocationed using the stack.
 * The purpose for this is because the stack, at least on my 
 * machine, runs 3000% faster than the heap (malloc).
 */
typedef struct Scope Scope;

struct Scope{
	int nodeSpot;
	int nodeLength;
	int scopeLength;
	int scopeSpot;
	ASTNode* nodes;

	/**
	 * Since we are trying to reduce the amount of heap
	 * memory we will allocate, we are using an array of
	 * scope ids. Each statement node will be assigned
	 * an ID from this scopes array. We will determine if
	 * a certain node belongs to a certain scope. This
	 * reduces the amount of memory required to build
	 * each scope.
	 */
	int* scopes;
};

void DestroyScope(Scope* scope);
void InitScope(Scope* scope);
void ExpandScope(Scope* scope, int tab);
void ExpandSubScope(Scope* scope, int id, int tab);

#endif // SCOPE_H_
