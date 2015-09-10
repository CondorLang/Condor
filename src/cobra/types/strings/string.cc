#include "string.h"

namespace Cobra {
namespace internal{

	void String::CB(Isolate* isolate){
		Cobra::Isolate* iso = CAST(Cobra::Isolate*, isolate);
		std::string hex = StringBytes;
		int len = hex.length();
		std::string newString;
		for(int i=0; i< len; i+=2)
		{
		    std::string byte = hex.substr(i,2);
		    char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
		    newString.push_back(chr);
		}
		Cobra::Handle* str = Cobra::String::New(iso, newString.c_str());
		Handle* iHandle = CAST(Handle*, str);
		String* iStr = iHandle->ToString();
		iStr->SetInternal();
		
		Cobra::Handle* script = Cobra::Script::Compile(iso, str);
		script->Run();
		// if (!script->IsCompiled()){
		// 	printf("d: %s\n", script->GetErrorMessage());
		// }
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