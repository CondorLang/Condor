#ifndef TOKENS_H_
#define TOKENS_H_

#include <string>

namespace Cobra {
	namespace Tokens {

		enum TOKEN_TYPES{
			KEYWORD,
			IDENTIFIER,
			CONSTANT,
			STRING,
			DELIMITER,
			OPERATOR,
			PUNCTUATOR,
			END
		};

		struct Token {
			TOKEN_TYPES type;
			std::string lexeme;
		};

	}
}

#endif // TOKENS_H_
