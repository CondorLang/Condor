// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The purpose of this file is to be a lightweight
 * runner for the AST
 */
#ifndef RUNNER_H_
#define RUNNER_H_

#include "../ast/scope.h"
#include "../ast/ast.h"
#include "../ast/astlist.h"
#include "utils/debug.h"

typedef struct RunnerContext {
  ASTNode* node;
  Token dataType;

  union {

    struct {
			bool value;
		} booleanValue;

		struct {
			signed char value;
		} byteValue;

		struct {
			short value;
		} shortValue;

		struct {
			int value;
		} intValue;

		struct {
			float value;
		} floatValue;

		struct {
			double value;
		} doubleValue;

		struct {
			long value;
		} longValue;

		struct {
			char value;
		} charValue;

		struct {
			char* value;
		} stringValue;

  } value;
} RunnerContext;

typedef struct Runner {
  Scope* scope;
  ASTNode* currentNode;
  RunnerContext* contexts;

  int totalContexts;
  int contextSpot;
} Runner;

/**
 * Public Functions
 */
void InitRunner(Runner* runner, Scope* scope);
void Run(Runner* runner, int scopeId);

/**
 * Private functions
 */
void RunStatement(Runner* runner);
void RunFuncCall(Runner* runner);
void RunFuncWithArgs(Runner* runner, ASTNode* func, ASTList* args);
void SetNodeValue(Runner* runner, ASTNode* node);
RunnerContext* GetNextContext(Runner* runner);

#endif // RUNNER_H_
