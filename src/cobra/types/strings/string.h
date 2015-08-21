#ifndef STRING_H_
#define STRING_H_

#include <string>

namespace Cobra {
namespace internal{

	class String
	{
	private:
		std::string value;
		std::string filePath;
	public:
		String(){value = "";}
		~String(){}
		void SetValue(const char* val){value = val;}
		const char* GetValue(){return value.c_str();}
		void SetPath(const char* path){filePath = path;}
		const char* GetPath(){return filePath.c_str();}
		int Length(){return value.length();}
		static bool Replace(std::string& str, const std::string& from, const std::string& to) {
	    size_t start_pos = str.find(from);
	    if(start_pos == std::string::npos)
	        return false;
	    str.replace(start_pos, from.length(), to);
	    return true;
		}
		static int NthSubStr(int n, const std::string& s, const std::string& p){
		   std::string::size_type i = s.find(p);
		   int j;
		   for (j = 1; j < n && i != std::string::npos; ++j)
		      i = s.find(p, i+1); 
		   if (j == n)
		     return(i);
		   else
		     return(-1);
		}
	};

} // namespace internal
}

#endif // STRING_H_
