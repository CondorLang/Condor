#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <vector>
#include "cobra/token/token.h"
#include "cobra/error/error.h"

namespace Cobra {
namespace internal{

	/**
	 * @brief Tokenize the program text
	 * @details Converts each character to a token based
	 * on the syntax of the language
	 * 
	 * @param source std::string
	 */
	class Scanner {
	public:
		Scanner(std::string* source);
		~Scanner(){}
		std::string* src;
		std::vector<std::string> errors;
		std::string result;
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
