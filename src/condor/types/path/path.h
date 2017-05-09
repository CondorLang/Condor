// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef PATH_H_
#define PATH_H_

#include <string>
#include <stdio.h>  /* defines FILENAME_MAX */

#include "platform/windows.h" // windows 

#ifdef _WIN32
	#include <Windows.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#include <limits.h> /* PATH_MAX */	
	#define GetCurrentDir getcwd
#endif

#include "condor/mem/isolate.h"

namespace Condor {
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
