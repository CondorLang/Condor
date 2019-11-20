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
#include "./runner-math.h"
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
RunnerContext* Run(Runner* runner, int scopeId);

/**
 * Private functions
 */
RunnerContext* RunStatement(Runner* runner);
RunnerContext* RunFuncCall(Runner* runner);
RunnerContext* RunFuncWithArgs(Runner* runner, ASTNode* func, ASTList* args);
RunnerContext* SetNodeValue(Runner* runner, ASTNode* node);
RunnerContext* GetNextContext(Runner* runner);
RunnerContext* GetContextByNodeId(Runner* runner, int nodeId);
RunnerContext* RunBinary(Runner* runner);
void RunSetVarType(Runner* runner, RunnerContext* context, ASTNode* node);
RunnerContext* RunMathContexts(RunnerContext* left, RunnerContext* right, Token op);
void PrintContext(RunnerContext* context);

#define CONTEXT_BOOLEAN_VALUE(node) node->value.booleanValue.value
#define CONTEXT_INT_VALUE(node) node->value.intValue.value

#endif // RUNNER_H_
