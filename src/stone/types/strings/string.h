// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef STRING_H_
#define STRING_H_

#include <string>

#include "../include/Stone.h"
#include "stone/mem/isolate.h"

namespace Stone {
namespace internal{

	class String
	{
	private:
		std::string value;
		std::string filePath;
		bool internal;
	public:
		String(){}
		static String* New(Isolate* isolate);
		void SetDefaults(){internal = false;}
		Isolate* isolate;
		std::string name;
		static std::string Code;
		void SetValue(const char* val);
		std::string GetValue(){return value;}
		void SetPath(const char* path){filePath = path;}
		const char* GetPath(){return filePath.c_str();}
		std::string GetPathStr(){return filePath;}
		int Length(){return value.length();}
		bool IsEmpty(){return value.empty() || Length() == 0;}
		static bool Replace(std::string& str, const std::string& from, const std::string& to);
		static int NthSubStr(int n, const std::string& s, const std::string& p);
		static std::vector<std::string> Split(std::string str, char delimiter);
		void SetInternal(){internal = true;}
		bool IsInternal(){return internal;}
	};

	namespace Sizes{
		const int kString = sizeof(String);
	}

} // namespace internal
}

#endif // STRING_H_