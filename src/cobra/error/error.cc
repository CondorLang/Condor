#include "error.h"

namespace Cobra {
namespace Error {
	std::string String(ERROR e, Token* tok){
		int err = (int) e;
		switch (err){
			case INVALID_MODE: return "Invalid mode";
			case EXPECTED_MODE: return "Expected mode";
			case MODE_UNKNOWN: return "Mode unknown";
			case EXPECTED_VARIABLE_TYPE: return "Expected variable type";
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
} // namespace Cobra