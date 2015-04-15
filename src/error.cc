#include "error.h"

namespace Cobra{
	namespace Errors {
		const char* toString(int e){
			switch (e){
				case EXPECTED_FUNCTION_NAME: return "Expected function name";
				case EXPECTED_LEFT_PARENTHESE: return "Expected (";
				case UNEXPECTED_CHARACTER: return "Unexpected character";
				case EXPECTED_LEFT_BRACE: return "Expected {";
				default: return "Unknown";
			}
		}
	}
}