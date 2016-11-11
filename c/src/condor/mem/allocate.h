// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef ALLOCATE_H_
#define ALLOCATE_H_

#include <stdlib.h>

void* Allocate(int size);
void Free(void* ptr);

#endif // ALLOCATE_H_
