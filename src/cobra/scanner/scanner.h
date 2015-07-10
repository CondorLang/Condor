#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <vector>
#include "../token/token.h"
#include "../error/error.h"

namespace Cobra {
	class Scanner {
	public:
		Scanner(std::string* source);
		~Scanner(){delete src;}
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
}

#endif // SCANNER_H_
