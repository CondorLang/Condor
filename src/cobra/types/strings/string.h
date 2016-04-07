#ifndef STRING_H_
#define STRING_H_

#include <string>

#include "../include/Cobra.h"
#include "cobra/mem/isolate.h"
#include "string-bytes.h"
#include "cobra/types/script/script.h"

namespace Cobra {
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
		static void CB(Isolate* isolate, std::string sub);
		static std::string Code;
		void SetValue(const char* val){value = val;}
		std::string GetValue(){return value;}
		void SetPath(const char* path){filePath = path;}
		const char* GetPath(){return filePath.c_str();}
		std::string GetPathStr(){return filePath;}
		int Length(){return value.length();}
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
