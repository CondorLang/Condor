#ifndef ERROR_H_
#define ERROR_H_

#include <string>
#include "../token/token.h"

namespace Cobra {
	namespace Error {
		enum ERROR {
			INVALID_MODE,
			EXPECTED
		};

		std::string String(ERROR e, Token* tok);
	}
}

#endif // ERROR_H_
