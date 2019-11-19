// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SEMANTIC_H_
#define SEMANTIC_H_

#include <stdio.h>
#include "utils/clock.h" // For debugging
#include "utils/debug.h"

#include "../ast/scope.h"
#include "../syntax/syntax.h"
#include "../token/token.h"
#include "../ast/astlist.h"
#include "../runner/runner.h"
#include "typechecker.h"
#include "utils/file/file.h"

void EnsureSemantics(Scope* scope, int scopeId);
void EnsureSemanticsForBody(Scope* scope, int scopeId);

void EnsureSemanticsForNode(Scope* scope, ASTNode* node);

/**
 * Used to predict VAR type. Algorithm goes as follows:
 * 	1. String literal = string
 * 	2. number literal = number
 */
void PredictVarType(ASTNode* node);

/**
 * Used to predict RETURN type. Return type will
 * determine what the function return type is.
 */
void PredictReturnType(ASTNode* node);

// TODO: Remove
void Scan(char* rawSourceCode);
void BuildTree(char* rawSourceCode);

#endif // SEMANTIC_H_
