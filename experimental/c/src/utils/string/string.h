// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef STRING_H_
#define STRING_H_

#include <stdlib.h>
#include <string.h>
#include "condor/mem/allocate.h"

/**
 * freeWhich:
 * 	1 = left
 * 	2 = right
 * 	0 = none
 */
char* Concat(const char* left, const char* right);

#endif // STRING_H_
