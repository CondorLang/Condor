// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "scanner.h"
#include "condor/token/token.h"
#include "condor/mem/isolate.h"

namespace Condor{
namespace internal{

	Scanner* Scanner::New(Isolate* iso, std::string* source){
		if (source == NULL) throw Error::EMPTY_FILE;
		void* p = iso->GetMemory(sizeof(Scanner));
		Scanner* s = new(p) Scanner();
		s->src = source;
		s->offset = -1;
		s->readOffset = 0;
		s->row = 1;
		s->col = 0;
		s->ch = -1;
		s->isolate = iso;
		return s;
	}

	/**
	 * @brief Get the next token
	 * @details Returns the next token. If it's the end of the file, 
	 * Token(END) will	 be returned
	 * @return Token*
	 */
	Token* Scanner::NextToken(){
		Next();
		while (ch == '\n' || ch == ' ' || ch == '\t' || ch == '\r') ScanWhiteSpaces();
		switch (ch){
			case -1: case '\0': return Token::New(isolate, END);
			case '/': {
				char p = Peek();
				if (p == '*'){
					ScanComments(true);
					return NextToken();
				}
				else if (p == '/'){
					ScanComments(false);
					return NextToken();
				}
				else if (p == '='){
					Next(); return Token::New(isolate, DIV_ASSIGN);
				}
				else {
					Next(); return Token::New(isolate, DIV);
				}
			}
			case '@':	return Token::New(isolate, CONSTRUCTOR);
			case '`':	return Token::New(isolate, TICK);
			case '$':	return Token::New(isolate, DOLLAR);
			case '*': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return Token::New(isolate, MUL_ASSIGN);
				}
				return Token::New(isolate, MUL);
			}
			case '+': {
				char p = Peek();
				if (p == '+') {
					Next(); 
					return Token::New(isolate, INC);
				}
				else if (p == '=') {
					Next(); 
					return Token::New(isolate, ADD_ASSIGN);
				}
				else return Token::New(isolate, ADD);
			}
			case '-': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return Token::New(isolate, SUB_ASSIGN);
				}
				else if (p == '-') {
					Next(); 
					return Token::New(isolate, DEC);
				}
				else if (p == '>') {
					Next(); 
					return Token::New(isolate, RARROW);
				}
				else if (IsNumber(p)){
					result = ch;
					char p = Peek();
					bool isInt = true;
					int count = 0;
					while (IsNumber(p) || p == '.'){
						if (!isInt) count++;
						if (ch == '.') isInt = false;
						Next();
						result += ch;
						p = Peek();
					}
					Token* tok = NULL;
					if (isInt) tok = Token::New(isolate, INT);
					else if (count < 7) tok = Token::New(isolate, FLOAT);
					else tok = Token::New(isolate, DOUBLE);
					tok->raw = result;
					return tok;
				}
				else return Token::New(isolate, SUB);
			}
			case '%': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return Token::New(isolate, MOD_ASSIGN);
				}
				else if (IsLetter(p) || p == '_'){ // internal
					std::string result = "";
					result += ch;
					Next();
					while (IsLetter(ch) || IsNumber(ch) || ch == '_'){
						result += ch;
						Next();
					}
					Token* tok = Token::New(isolate, INTERNAL);
					tok->raw = result;
					return tok;
				}
				return Token::New(isolate, MOD);
			}
			case '<': {
				char p = Peek();
				if (p == '-') {
					Next(); 
					return Token::New(isolate, LARROW);
				}
				else if (p == '<') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						Next(); 
						return Token::New(isolate, SHL_ASSIGN);
					}
					Next();
					return Token::New(isolate, SHL);
				}
				else if (p == '=') {
					Next(); 
					return Token::New(isolate, LEQ);
				}
				else return Token::New(isolate, LSS);
			}
			case '>': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return Token::New(isolate, GEQ);
				}
				if (p == '>') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						Next(); 
						return Token::New(isolate, SHR_ASSIGN);
					}
					Next();
					return Token::New(isolate, SHR);
				}
				else return Token::New(isolate, GTR);
			}
			case '!': {
				char p = Peek();
				if (p == '='){
					Next();
					return Token::New(isolate, NEQ);
				}
				return Token::New(isolate, NOT);
			}
			case '&': {
				char p = Peek();
				if (p == '&') {
					Next(); 
					return Token::New(isolate, LAND);
				}
				else if (p == '^') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						return Token::New(isolate, AND_NOT_ASSIGN);
					}
					Next();
					return Token::New(isolate, AND_NOT);
				}
				else if (p == '=') {
					Next();
					return Token::New(isolate, AND_ASSIGN);
				}
				return Token::New(isolate, AND);
			}
			case '|': {
				char p = Peek();
				if (p == '|') {
					Next(); 
					return Token::New(isolate, LOR);
				}
				else if (p == '=') {
					Next(); 
					return Token::New(isolate, OR_ASSIGN);
				}
				return Token::New(isolate, OR);
			}
			case '^': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return Token::New(isolate, XOR_ASSIGN);
				}
				return Token::New(isolate, POW);
			}
			case '=': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return Token::New(isolate, EQL);
				}
				return Token::New(isolate, ASSIGN);
			}
			case ';': return Token::New(isolate, SEMICOLON);
			case ':': return Token::New(isolate, COLON);
			case ',': return Token::New(isolate, COMMA);
			case '.': {
				char p = Peek();
				char p2 = src->at(readOffset + 1);
				if (p == '.' && p2 == '.') {
					Next();
					Next();
					return Token::New(isolate, ELLIPSIS);
				}
				return Token::New(isolate, PERIOD);
			}
			case '(': return Token::New(isolate, LPAREN);
			case ')': return Token::New(isolate, RPAREN);;
			case '[': return Token::New(isolate, LBRACK);
			case ']': return Token::New(isolate, RBRACK);
			case '{': return Token::New(isolate, LBRACE);
			case '}': return Token::New(isolate, RBRACE);
			case '#': return Token::New(isolate, HASH);
			case '\'': {
				char p = Peek();
				CheckEOF(p);
				char p2 = src->at(readOffset + 1);
				if (p == '\\'){
					Next();
					Token* tok = Token::New(isolate, CHAR);
					tok->raw = ScanEscape();
					Next();
					return tok;
				}
				else if (p2 == '\''){
					Token* tok = Token::New(isolate, CHAR);
					tok->raw = p;
					Next();
					Next();
					return tok;
				}
				else if (p == '\''){
					Next();
					return Token::New(isolate, CHAR);
				}
				throw Error::INVALID_CHAR_VALUE;
			}
			case '"': {
				Next(); // eat "
				if (ch == '"') result = "";
				else{
					std::string s = ScanEscape(); // first char escaped
					if (s.length() > 0) result = s;
					if (s.length() == 0) result = ch;

					while (true){
						Next();
						s = ScanEscape();
						if (s.length() > 0) result += s;
						if (ch == '"' || ch == -1) break;
						if (s.length() == 0) result += ch;
					}
				}
				Token* tok = Token::New(isolate, STRING);
				tok->raw = result;
				return tok;
			}
			default: {
				if (IsLetter(-1) || ch == '_' || ch == '$'){
					result = ch;
					char p = Peek();
					while ((IsLetter(p) || IsNumber(p) || p == '_') && p != ' ' && p != 0){
						Next();
						result += ch;
						p = Peek();
					}
					Token* tok = Token::GetToken(isolate, result);
					if (tok->value == IDENT){
						tok->raw = result;
					}
					else if (tok->value == ILLEGAL) return NULL;
					return tok;
				}
				else if (IsNumber(-1)){
					result = ch;
					char p = Peek();
					bool isInt = true;
					int count = 0;
					while (IsNumber(p) || p == '.'){
						if (!isInt) count++;
						if (ch == '.') isInt = false;
						Next();
						result += ch;
						p = Peek();
					}
					Token* tok = NULL;
					if (isInt) tok = Token::New(isolate, INT);
					else if (count < 7) tok = Token::New(isolate, FLOAT);
					else tok = Token::New(isolate, DOUBLE);
					tok->raw = result;
					return tok;
				}
			}

			return NULL;
		}
	}

	/**
	 * @brief Register an Error
	 * @details If there are errors in the code,
	 * an error will be stored here.
	 * 
	 * @param err std::string
	 */
	void Scanner::Error(std::string err){
		err += row;
		err += ":";
		err += offset;
		errors.push_back(err);
	}

	/**
	 * @brief Is Letter
	 * @details Checks if the character is a letter
	 * 
	 * @param letter char
	 * @return bool
	 */
	bool Scanner::IsLetter(char letter){
		if (letter != -1) return isalpha(letter);
		return isalpha(ch);
	}

	/**
	 *
	 */
	void Scanner::CheckEOF(char c){
		if (c == -1) throw Error::UNEXPECTED_END_OF_FILE;
	}

	/**
	 * @brief Is Number
	 * @details Checks if the character is a number
	 * 
	 * @param letter char
	 * @return bool
	 */
	bool Scanner::IsNumber(char letter){
		if (letter != -1) return isdigit(letter);
		return isdigit(ch);
	}

	/**
	 * @brief Next character
	 * @details Grabs the next character or eats whitespaces
	 * and new lines
	 */
	void Scanner::Next(){
		if (readOffset < src->length()){
			offset = readOffset;
			if (ch == '\n') {
				row++;
				col = 1;
				ch = src->at(offset);
				readOffset++;
			}
			else{
				ch = src->at(offset);
				if (ch == 0) Error("Illegal null");
				readOffset++;
				col++;
			}
		}
		else{
			offset = src->length();
			if (ch == '\n') row++;
			ch = -1; // EOF
		}
	}

	/**
	 * @brief Peek at next character
	 * @details Peeks at the next character. Returns -1 if it's
	 * the end of the file
	 * @return char
	 */
	char Scanner::Peek(){
		if (readOffset < src->length()){
			return src->at(readOffset);
		}
		return -1; // EOF
	}

	/**
	 * @brief Eat comments
	 * 
	 * @param star true if / * or false if //
	 */
	void Scanner::ScanComments(bool star){
		Next(); // eat the second comment char
		char p = Peek();
		if (star){
			while (!(ch == '*' && p == '/') && ch != -1){
				Next();
				p = Peek();
			}
			Next(); // eat *
		}
		else{
			while (p != '\n' && p != -1){
				Next();
				p = Peek();
			}
		}
		ScanWhiteSpaces();
	}

	/**
	 * @brief Eat white spaces
	 */
	void Scanner::ScanWhiteSpaces(){
		if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'){
			char p = Peek();
			Next();
			while (true){			
				if (p != ' ' || p != '\t' || p != '\n' || p != -1 || p != '\r')
					break;
				p = Peek();
				Next();
			}
		}
	}

	std::string Scanner::ScanEscape(){
		std::string result = "";
		if (ch == '\\'){
			Next();
			// TODO: Include Octal, Hex, UTF-8, UTF-16, see https://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
			switch (ch){
				case 'n': result += '\n'; break;
				case 't': result += '\t'; break;
				case 'a': result += '\a'; break;
				case 'b': result += '\b'; break;
				case 'f': result += '\f'; break;
				case 'r': result += '\r'; break;
				case 'v': result += '\v'; break;
				case '\\': result += '\\'; break;
				case '0': { // octal
					Next();
					if (ch == '8' || ch == '9') throw Error::ILLEGAL_OCTAL_CODE;
					std::string num;
					num += ch;
					Next();
					num += ch;
					int times = std::stoi(num);

					int octal = times - (((times - (times % 10)) / 10) * 2);
					result += ((char) octal);
					break;
				}
				case '1': result += '\1'; break;
				case '2': result += '\2'; break;
				case '3': result += '\3'; break;
				case '4': result += '\4'; break;
				case '5': result += '\5'; break;
				case '6': result += '\6'; break;
				case '7': result += '\7'; break;
				case '?': result += '\?'; break;
				case '\'': result += '\''; break;
				case '"': result += '\"'; break;
				default: throw Error::UNKNWON_ESCAPE_SEQUENCE;
			}
		}
		return result;
	}

	std::string Scanner::Substr(int start, int end){
		return src->substr(start, end - start);
	}

	std::string Scanner::LookAhead(int len){
		return Substr(offset, offset + len);
	}
} // namespace internal
} // namespace Condor