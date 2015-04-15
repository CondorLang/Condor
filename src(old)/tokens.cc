#include "tokens.h"

namespace Cobra{
	namespace Tokens{
		bool isReservedWord(const char* str){
			char result[25];
			strcpy(result, " ");
			strcat(result, str);
			strcat(result, " ");
			return strstr(RESERVED_WORDS, result) != NULL;
		}
	}
}