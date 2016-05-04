// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef APP_H_
#define APP_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "app-bytes.h"

namespace Cobra {
namespace internal{

	class App
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
		static bool Included;
	};

} // namespace internal
}

#endif // APP_H_
