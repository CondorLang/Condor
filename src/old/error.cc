#include "error.h"

namespace Cobra{
	namespace Errors {
		const char* toString(int e){
			switch (e){
				case EXPECTED_FUNCTION_NAME: return "Expected function name";
				case EXPECTED_LEFT_PARENTHESE: return "Expected (";
				case UNEXPECTED_CHARACTER: return "Unexpected character";
				case EXPECTED_LEFT_BRACE: return "Expected {";
				case FILE_IO: return "Unable to read file";
				case EXPECTED_RIGHT_BRACE: return "Expected {";
				case MAIN_NOT_FOUND: return "Main function not found";
				case UNDEFINED: return "Undefined";
				case MAX_ARG_LIMIT: return "Max argument limit";
				case EXPRECTED_OBJECT_NAME: return "Expected object name";
				case EXPECTED_EQUALS: return "Expected =";
				case MISSING_SEMI_COLON: return "Missing ;";
				case EXPECTED_OPERATOR: return "Expected operator";
				case FUNCTION_NOT_FOUND: return "Function not found";
				case ILLEGAL_STRING_CONCATENATION: return "Illegal string concatenation";
				default: return "Unknown";
			}
		}
	}
}