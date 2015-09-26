#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <vector>
#include "cobra/token/token.h"
#include "cobra/error/error.h"
#include "cobra/types/vector/vector.h"

namespace Cobra {
namespace internal{

	class Isolate;

	/**
	 * @brief Tokenize the program text
	 * @details Converts each character to a token based
	 * on the syntax of the language
	 * 
	 * @param source std::string
	 */
	class Scanner {
	public:
		static Scanner* New(Isolate* iso, std::string* source);
		std::string* src;
		Vector<std::string> errors;
		std::string result;
		Isolate* isolate;
		char ch;
		int offset;
		int readOffset;
		int row;
		int col;

		Token* NextToken();
		bool IsLetter(char letter);
		bool IsNumber(char letter);
		void Next();
		char Peek();
		void Error(std::string err);
		void ScanComments(bool star);
		void ScanWhiteSpaces();
	};
} // namespace internal{
}

#endif // SCANNER_H_
