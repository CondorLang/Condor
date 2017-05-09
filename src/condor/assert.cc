#include "assert.h"
#include <stdio.h> 

// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

namespace Condor {
namespace internal{
	
	void FATAL(const char* file, int line, const char* msg1, const char* msg2){
		printf("File: %s, Line: %d, %s %s\n", file, line, msg1, msg2); 
		Isolate::CurrentIsolate->Dispose();
		exit(1);
	}

} // namespace internal
} // namespace Condor