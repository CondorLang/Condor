#ifndef ERROR_H_
#define ERROR_H_

namespace Cobra {
	namespace Errors {

		enum ERROR_MSG {
			EXPECTED_FUNCTION_NAME = 1,
			EXPECTED_LEFT_PARENTHESE,
			UNEXPECTED_CHARACTER,
			EXPECTED_LEFT_BRACE
		};

		const char* toString(int e);

	}
}

#endif // ERROR_H_
