// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SEMANTIC_H_
#define SEMANTIC_H_

#include "utils/clock.h" // For debugging
#include "utils/debug.h"

#include "../ast/scope.h"
#include "../syntax/syntax.h"
#include "../token/token.h"

void EnsureSemantics(Scope* scope);

/**
 * Used to predict VAR type. Algorithm goes as follows:
 * 	1. String literal = string
 * 	2. number literal = number
 */
void PredictVarType(ASTNode* node);
Token GetBinaryType(ASTNode* node);

// TODO: Remove
void Scan(char* rawSourceCode);
void BuildTree(char* rawSourceCode);

#endif // SEMANTIC_H_
