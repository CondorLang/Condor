#include "tokens.h"

namespace Cobra{
	namespace Tokens{
		bool isReservedWord(const char* str){
			std::string word;
			word = str;

			std::string reserved;
			reserved = RESERVED_WORDS;

			return reserved.find(str) != std::string::npos;
		}
	}
}