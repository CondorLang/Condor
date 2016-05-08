// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "string.h"

namespace Cobra {
namespace internal{

	String* String::New(Isolate* isolate){
		void* p = (String*) isolate->GetMemory(sizeof(String));
		String* str = new(p) String();
		str->SetDefaults();
		return str;
	}

	bool String::Replace(std::string& str, const std::string& from, const std::string& to){
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
	}

	void String::SetValue(const char* val){
		value = val;
	}

	int String::NthSubStr(int n, const std::string& s, const std::string& p){
		std::string::size_type i = s.find(p);
		int j;
		for (j = 1; j < n && i != std::string::npos; ++j)
		  i = s.find(p, i+1); 
		if (j == n)
		 	return(i);
		else
		 	return(-1);
	}

	std::vector<std::string> String::Split(std::string str, char delimiter){
		std::vector<std::string> result;
		int start = 0;
		std::string tmp;
		for (int i = 0; i < str.length(); i++){
			if (str[i] == delimiter){
				tmp = str.substr(start, i - start);
				result.push_back(tmp);
				start = i + 1;
			}
		}
		result.push_back(str.substr(start, str.size() - start));
		return result;
	}

} // namespace internal
}