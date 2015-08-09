#ifndef STRING_H_
#define STRING_H_

#include <string>

namespace Cobra {
namespace internal{

	class String
	{
	private:
		std::string value;
		const char* filePath;
	public:
		String(){value = "";}
		~String(){}
		void SetValue(const char* val){value = val;}
		const char* GetValue(){return value.c_str();}
		void SetPath(const char* path){filePath = path;}
		const char* GetPath(){return filePath;}
		int Length(){return value.length();}
	};

} // namespace internal
}

#endif // STRING_H_
