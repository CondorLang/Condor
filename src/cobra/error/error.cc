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
			case PARSE_VAR_ERR: return "Parse var error";
			case UNKNOWN_CAST_TYPE: return "Unknown cast type";
			case UNIDENTIFIED_OBJECT_MEMBER: return "Unidentfied object member";
			case INVALID_FUNC_CALL: return "Invalid function call";
			case UNDEFINED_OBJECT: return "Undefined object";
			case UNABLE_TO_ACCESS_PRIVATE_MEMBER: return "Unable to access private member";
			case DIFFERENT_TYPE_ALREADY_DEFINED_IN_SCOPE: return "Different type already defined in scope";
			case COMPILATION_ERROR: return "Compilation error";
			case MULTIPLE_DECL_OF_OBJECT: return "Can't have multiple declarations of an object";
			case CANNOT_ADD_TO_ARRAY: return "Cannot add to array";
			case INVALID_ADD_EXPR: return "Invalid add expression";
			case CANNOT_SUB_CHAR_FROM_TYPE: return "Cannot subtract char from type";
			case CANNOT_SUB_STRING_FROM_TYPE: return "Cannot subtract string from type";
			case INVALID_SUB_EXPR: return "Invalid subtraction expression";
			case CANNOT_SUB_FROM_ARRAY: return "Cannot subtract from array";
			case CANNOT_DIV_CHAR_FROM_TYPE: return "Cannot divide char from type";
			case CANNOT_DIV_STRING_FROM_TYPE: return "Cannot divide string from type";
			case CANNOT_DIV_FROM_ARRAY: return "Cannot divide from array";
			case CANNOT_MULT_CHAR_FROM_TYPE: return "Cannot multiply char from type";
			case CANNOT_MULT_STRING_FROM_TYPE: return "Cannot multiply string from type";
			case CANNOT_MULT_FROM_ARRAY: return "Cannot multiply from array";
			case CANNOT_MOD_CHAR_FROM_TYPE: return "Cannot mod char from type";
			case CANNOT_MOD_STRING_FROM_TYPE: return "Cannot mod string from type";
			case CANNOT_MOD_FROM_ARRAY: return "Cannot mod from array";
			case CANNOT_DIV_INT_FROM_TYPE: return "Cannot divide int from type";
			case CANNOT_DIV_DOUBLE_FROM_TYPE: return "Cannot divide double from type";
			case CANNOT_DIV_FLOAT_FROM_TYPE: return "Cannot divide float from type";
			case CANNOT_MULT_INT_FROM_TYPE: return "Cannot multiply int from type";
			case CANNOT_MULT_DOUBLE_FROM_TYPE: return "Cannot multiply double from type";
			case CANNOT_MULT_FLOAT_FROM_TYPE: return "Cannot multiply float from type";
			case CANNOT_MOD_INT_FROM_TYPE: return "Cannot mod int from type";
			case CANNOT_MOD_DOUBLE_FROM_TYPE: return "Cannot mod double from type";
			case CANNOT_MOD_FLOAT_FROM_TYPE: return "Cannot mod float from type";
			case INVALID_MOD_EXPR: return "Invalid mod expression";
			case INVALID_DIV_EXPR: return "Invalid divide expression";
			case INVALID_MULT_EXPR: return "Invalid multiply expression";
			case INVALID_ASSIGNMENT_TO_TYPE: return "Invalid assignment to type";
			case INVALID_ARRAY_TYPE: return "Invalid array type";
			case INVALID_BOOLEAN_COMPARISON: return "Invalid boolean comparison";
			case INVALID_CONDITION_FOR_IF_STMT: return "Invalid condition for if statement";
			case UNEXPECTED_TICK: return "Unexpected tick";
			case UNEXPECTED_OPERATOR: return "Unexpected operator";
			case TOO_MANY_OBJECT_DEFINITIONS: return "Too many object definitions";
			case EXPECTED_OBJECT: return "Expected object";
			case OBJECT_CONSTRUCTOR_INVALID: return "Object constructor invalid";
			case UNABLE_TO_ACCESS_NON_STATIC_FUNCTION: return "Unable to access non static function";
			case INVALID_CATCH: return "Invalid catch";
			case INVALID_THROW: return "Invalid throw";
			case EMPTY_FILE: return "Empty file";
			case EMPTY_FILE_PARSER: return "Empty file in parser";
			case UNIDENTIFIED_BOOLEAN_OPERATOR: return "Unidentfied boolean operator";
			case INVALID_OBJECT_EXTENSION: return "Invalid object extension";
			case INVALID_OPERATOR: return "Invalid operator";
			case UNABLE_TO_ACCESS_STATIC_MEMBER: return "Unable to access static member";
			case EXPECTED_IDENTIFIER: return "Expected identifier";
			case EXPECTED_ARRAY_TYPE: return "Expected array type";
			case CORRUPT_OBJECT: return "Corrupt object";
			case EXPECTED_OBJECT_MEMBER_NAME: return "Expected object member name";
			case UNMATCHED_FUNCTION_PARAMETERS: return "Unmatched function parameter";
			case INVALID_IMPORT: return "Invalid import";
			case UNDEFINED_VARIABLE_TYPE: return "Undefined variable type";
			case CHECK_IMPORTS: return "Check imports";
			case UNEXPECTED_CHARACTER: return "Unexpected character";
			case INVALID_ARRAY_MEMBER: return "Invalid array member";
			case INVALID_INCLUDE_IMPORT: return "Invalid include or import";
			case UNEXPECTED_END_OF_FILE: return "Unexpected end of file";
			case INVALID_USE_OF_EXPORT: return "Invalid use of export";
			case INVALID_CASE_STMT: return "Invalid case statement";
			case NOT_IMPLEMENTED: return "Not implemented";
			case MULTIPLE_RETURNS_NOT_ALLOWED: return "Multiple returns not allowed";
			case INVALID_ACCESS_TO_ARRAY: return "Invalid access to array";
			case NO_RETURN_STMTS_IN_CONSTRUCTOR: return "No return statements are allowed in the constructor";
			case INVALID_USAGE_OF_THIS: return "Invalid usage of this";
			case RESERVED_KEYWORD: return "Reserved keyword";
			case INVALID_SCRIPT: return "Invalid script";
			case UNKNWON_ESCAPE_SEQUENCE: return "Unknown escape sequence";
			case INVALID_CAST: return "Invalid cast";
			case INTERNAL_BREAK: return "Internal break";
			case INVALID_USE_OF_BREAK: return "Invalid use of break";
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