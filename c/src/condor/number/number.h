// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef NUMBER_H_
#define NUMBER_H_

#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h> // for not_implemented function

#include "condor/token/token.h"
#include "utils/assert.h"

Token GetNumberType(char* value);

#endif // NUMBER_H_
