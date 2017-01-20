// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef ASSERT_H_
#define ASSERT_H_

#include "condor/token/token.h"

#define EXPECT_TOKEN(got, tok, lexer) if (tok != got) { \
	printf("Parse error: Expected: %s, but got: %s, at %d:%d\n", #tok, TokenToString(got), lexer->tracker.row, lexer->tracker.col + lexer->tracker.currentTokenPosition - 1); \
	exit(0); \
}

#define EXPECT_STRING(val) { printf("Expected: %s\n", val); exit(0); }
#define NOT_IMPLEMENTED(val) { printf("Not Implemented (%s:%d): %s\n", __FUNCTION__, __LINE__, val); exit(0); }
#define CHECK(condition) {if (!condition) {printf("Invalid Check: %s in %s:%d\n", #condition, __FUNCTION__, __LINE__);}}
#define TRACK() {if (false) {printf("%s:%d\n", __FUNCTION__, __LINE__);}}

#endif // ASSERT_H_
