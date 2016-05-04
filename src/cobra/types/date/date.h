// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef DATE_H_
#define DATE_H_

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "date-bytes.h"

namespace Cobra {
namespace internal{

	class Date
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
	};

} // namespace internal
}

#endif // DATE_H_
