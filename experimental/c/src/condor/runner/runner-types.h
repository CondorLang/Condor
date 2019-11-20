// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef RUNNER_TYPES_H_
#define RUNNER_TYPES_H_

#include "../ast/ast.h"

typedef struct RunnerContext {
  ASTNode* node;
  Token dataType;

  union {
		bool vBoolean;
		unsigned char vByte;
		short vShort;
		int vInt;
		float vFloat;
		double vDouble;
		long vLong;
		char vChar;
		char* vString;
	} value;

} RunnerContext;

typedef struct Runner {
  Scope* scope;
  ASTNode* currentNode;
  RunnerContext* contexts;

  int totalContexts;
  int contextSpot;
} Runner;

#endif // RUNNER_TYPES_H_