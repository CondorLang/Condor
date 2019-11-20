// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef RUNNER_MATH_H_
#define RUNNER_MATH_H_

#include <limits.h>
#include <float.h>
#include "../ast/ast.h"
#include "./runner-types.h"

double ContextToDouble(RunnerContext* context);
double RunMath(double left, double right, Token op);
void CastToType(RunnerContext* context);

#endif // RUNNER_MATH_H_