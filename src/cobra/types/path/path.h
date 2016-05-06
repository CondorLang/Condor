// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef PATH_H_
#define PATH_H_

#include <string>
#include <stdio.h>  /* defines FILENAME_MAX */

#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	class Path
	{
	private:
		Isolate* isolate;
		std::string base;

	public:
		Path(Isolate* iso);
		~Path(){}
		static Path* New(Isolate* iso);
		void SetBase(std::string str);
		std::string GetFromBase(std::string str);
		static std::string GetCWD();
		static std::string GetLibDir();

	};

} // namespace internal
}

#endif // PATH_H_
