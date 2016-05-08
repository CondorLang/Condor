// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef FS_H_
#define FS_H_

#include <fstream>
#include <streambuf>
#include <iostream>

namespace Cobra {
namespace internal{

	class FS
	{
	public:
		static bool FileExists(std::string path);
		static std::string ReadFile(std::string path);
		static void WriteFile(std::string path, std::string contents);
		static bool DeleteFile(std::string path);
	};

} // namespace internal
}

#endif // FS_H_
