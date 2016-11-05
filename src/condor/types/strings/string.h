// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef STRING_H_
#define STRING_H_

#include <string>
// #include <iomanip>
// #include <sstream>

#include "../include/Condor.h"
#include "condor/mem/isolate.h"

namespace Condor {
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
		void SetUniqueName();
		static int UniqueTotal;

		template <typename T>
		static std::string ToStringWithPercision(const T val, const int n = 15){
		    // std::ostringstream out;
		    // out << std::fixed;
		    // out << std::setprecision(n) << val;
		    // return out.str();
		    return std::to_string(val);
	  	}
	};

	namespace Sizes{
		const int kString = sizeof(String);
	}

} // namespace internal
}

#endif // STRING_H_
