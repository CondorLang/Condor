#include "error.h"

namespace Cobra {
namespace internal{
namespace Error {
	std::string String(ERROR e, Token* tok){
		int err = (int) e;
		switch (err){
			case INVALID_MODE: return "Invalid mode";
			case EXPECTED_MODE: return "Expected mode";
			case MODE_UNKNOWN: return "Mode unknown";
			case EXPECTED_VARIABLE_TYPE: return "Expected variable type";
			case INVALID_STMT: return "Invalid statement";
			case VAR_NOT_ALLOWED_IN_STRICT_MODE: return "var not allowed in strict mode";
			case EXPECTED_VAR: return "Expected var";
			case EXPECTED_OPERATOR: return "Expected operator";
			case INVALID_FUNCTION_CALL: return "Invalid function call";
			case MISSING_EXPR: return "Missing Expression";
			case INVALID_EXPR: return "Invalid Expression";
			case INVALID_LEFT_EXPR: return "Invalid left expression";
			case UNDEFINED_VARIABLE: return "Undefined variable";
			case INVALID_OBJECT_MEMBER: return "Invalid object member";
			case INVALID_CHAR_VALUE: return "Invalid char value";
			case EXPECTED_PARAMETER: return "Expected parameter";
			case UNEXPECTED_RETURN: return "Unexpected return";
			case EXPECTED_ARG_NAME: return "Expected argument name";
			case MISSING_FUNC_BODY: return "Missing func body";
			case INTERNAL_SCOPE_ERROR: return "Internal scope error";
			case UNDEFINED_FUNC: return "Undefined func";
			case INVALID_ARGUMENT_TYPE: return "Invalid argument type";
			case CANT_HAVE_NULL_FUNC_ARG: return "Can't have null func arg";
			case CANNOT_CONCAT_TYPE: return "Cannot concatenate type";
			case CANNOT_SUBTRACT: return "Cannot subtract type";
			case CANNOT_MINUS_UNLIKE_VALUES: return "Cannot minus unlike values";
			case CANNOT_ADD_UNLIKE_VALUES: return "Cannot add unlike values";
			case PARSE_VAR_ERR: return "Parser var error";
			case SCANNER: {
				if (tok != NULL){
					return tok->raw;
				}
			}
			case UNDEFINED: return "Undefined";
			case EXPECTED: {
				if (tok != NULL){
					std::string msg = "Expected ";
					msg += tok->String();
					return msg;
				}
				return "Expected something";
			}
			default: return "Undefined";
		}
	}
} // namespace Error
} // namespace internal{
} // namespace Cobra