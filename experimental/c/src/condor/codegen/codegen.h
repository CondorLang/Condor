// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "condor/token/token.h"
#include "condor/mem/allocate.h"

int CodeGen_Add(int a, int b);
int CodeGen_Sub(int a, int b);
char* CodeGen_DeclareVariable(char* name, Token dataType);

#endif // CODEGEN_H_
