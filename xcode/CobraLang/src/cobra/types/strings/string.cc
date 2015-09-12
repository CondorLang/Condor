#include "string.h"

namespace Cobra {
namespace internal{

	void String::CB(Isolate* isolate){
		Script::RunInternalScript(isolate, StringBytes);
	}

	bool String::Replace(std::string& str, const std::string& from, const std::string& to){
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
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

} // namespace internal
}