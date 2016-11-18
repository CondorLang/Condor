// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <vector>
#include "condor/token/token.h"
#include "condor/error/error.h"

namespace Condor {
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
		Scanner(){}
		~Scanner(){}
		static Scanner* New(Isolate* iso, std::string* source);
		std::string* src;
		std::vector<std::string> errors;
		std::string result;
		Isolate* isolate;
		char ch;
		int offset;
		unsigned int readOffset;
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
		void CheckEOF(char c);
		std::string ScanEscape();
		std::string LookAhead(int len);
		std::string Substr(int start, int end);
	};
} // namespace internal{
}

#endif // SCANNER_H_
