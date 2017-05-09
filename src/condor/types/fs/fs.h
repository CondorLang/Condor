// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef FS_H_
#define FS_H_

#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <dirent.h>
#include "condor/error/error.h"

namespace Condor {
namespace internal{

	class FS
	{
	public:
		static bool FileExists(std::string path);
		static std::string ReadFile(std::string path);
		static void WriteFile(std::string path, std::string contents);
		static bool DeleteFile(std::string path);
		static std::vector<std::string> ReadDir(std::string path, bool recursive);
		static bool IsDir(std::string path);
	};

} // namespace internal
}

#endif // FS_H_
