// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef FS_H_
#define FS_H_

#include <fstream>
#include <streambuf>
#include <iostream>

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "fs-bytes.h"

namespace Cobra {
namespace internal{

	class FS
	{
	public:
		static void CB(Isolate* isolate, std::string sub);
		static bool FileExists(std::string path);
		static std::string ReadFile(std::string path);
		static void WriteFile(std::string path, std::string contents);
	};

} // namespace internal
}

#endif // FS_H_
