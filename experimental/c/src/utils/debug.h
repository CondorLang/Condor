// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef DEBUG_H_
#define DEBUG_H_

#include <string.h>

#if DEBUG == 1
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	
	/**
	 * Debugging support. If the DEBUG_NAMESPACE variable is the filename, then it will print
	 */
	#ifndef DEBUG_NAMESPACE
		#define DEBUG_NAMESPACE "All"
		#define SHOULD_PRINT(file) true
	#else
		#define SHOULD_PRINT(file) (strcmp(DEBUG_NAMESPACE, __FILENAME__) == 0 || strcmp(DEBUG_NAMESPACE, "All") == 0)
	#endif

	#define DEBUG_PRINT(a) if (SHOULD_PRINT(__FILENAME__)) printf("%s:%d - %s\n", __FILENAME__, __LINE__, a);
	#define DEBUG_PRINT2(a, b) if (SHOULD_PRINT(__FILENAME__)) printf("%s:%d - %s - %s\n", __FILENAME__, __LINE__, a, b);
	#define DEBUG_PRINT3(a, b, c) if (SHOULD_PRINT(__FILENAME__)) printf("%s:%d - %s - %s - %s\n", __FILENAME__, __LINE__, a, b, c);

#else	 
	#define DEBUG_PRINT(a) 
	#define DEBUG_PRINT2(a, b)
	#define DEBUG_PRINT3(a, b, c)
#endif

#endif // DEBUG_H_
