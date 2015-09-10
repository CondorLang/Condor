#include "scanner.h"
#include "cobra/token/token.h"
#include "cobra/mem/isolate.h"

namespace Cobra{
namespace internal{

	/**
	 * @brief Scanner constructor
	 * 
	 * @param source std::string
	 */
	Scanner::Scanner(Isolate* iso, std::string* source){
		src = source;
		offset = -1;
		readOffset = 0;
		row = 1;
		col = 0;
		ch = -1;
		isolate = iso;
	}

	/**
	 * @brief Get the next token
	 * @details Returns the next token. If it's the end of the file, 
	 * Token(END) will be returned
	 * @return Token*
	 */
	Token* Scanner::NextToken(){
		Next();
		
		while (ch == '\n' || ch == ' ' || ch == '\t')
			ScanWhiteSpaces();

		switch (ch){
			case -1: return isolate->InsertToHeap(new Token(END), INTERNAL_TOKEN);
			case '\0': return isolate->InsertToHeap(new Token(END), INTERNAL_TOKEN);
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
					Next(); return isolate->InsertToHeap(new Token(DIV_ASSIGN), INTERNAL_TOKEN);
				}
				else {
					Next(); return isolate->InsertToHeap(new Token(DIV), INTERNAL_TOKEN);
				}
			}
			case '@': {
				return isolate->InsertToHeap(new Token(CONSTRUCTOR), INTERNAL_TOKEN);
			}
			case '`': {
				return isolate->InsertToHeap(new Token(TICK), INTERNAL_TOKEN);
			}
			case '*': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(MUL_ASSIGN), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(MUL), INTERNAL_TOKEN);
			}
			case '+': {
				char p = Peek();
				if (p == '+') {
					Next(); 
					return isolate->InsertToHeap(new Token(INC), INTERNAL_TOKEN);
				}
				else if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(ADD_ASSIGN), INTERNAL_TOKEN);
				}
				else return isolate->InsertToHeap(new Token(ADD), INTERNAL_TOKEN);
			}
			case '-': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(SUB_ASSIGN), INTERNAL_TOKEN);
				}
				else if (p == '-') {
					Next(); 
					return isolate->InsertToHeap(new Token(DEC), INTERNAL_TOKEN);
				}
				else if (p == '>') {
					Next(); 
					return isolate->InsertToHeap(new Token(RARROW), INTERNAL_TOKEN);
				}
				else return isolate->InsertToHeap(new Token(SUB), INTERNAL_TOKEN);
			}
			case '%': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(MOD_ASSIGN), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(MOD), INTERNAL_TOKEN);
			}
			case '<': {
				char p = Peek();
				if (p == '-') {
					Next(); 
					return isolate->InsertToHeap(new Token(LARROW), INTERNAL_TOKEN);
				}
				else if (p == '<') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						Next(); 
						return isolate->InsertToHeap(new Token(SHL_ASSIGN), INTERNAL_TOKEN);
					}
					Next();
					return isolate->InsertToHeap(new Token(SHL), INTERNAL_TOKEN);
				}
				else if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(LEQ), INTERNAL_TOKEN);
				}
				else return isolate->InsertToHeap(new Token(LSS), INTERNAL_TOKEN);
			}
			case '>': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(GEQ), INTERNAL_TOKEN);
				}
				if (p == '>') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						Next(); 
						return isolate->InsertToHeap(new Token(SHR_ASSIGN), INTERNAL_TOKEN);
					}
					Next();
					return isolate->InsertToHeap(new Token(SHR), INTERNAL_TOKEN);
				}
				else return isolate->InsertToHeap(new Token(GTR), INTERNAL_TOKEN);
			}
			case '!': {
				char p = Peek();
				if (p == '='){
					Next();
					return isolate->InsertToHeap(new Token(NEQ), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(NOT), INTERNAL_TOKEN);
			}
			case '&': {
				char p = Peek();
				if (p == '&') {
					Next(); 
					return isolate->InsertToHeap(new Token(LAND), INTERNAL_TOKEN);
				}
				else if (p == '^') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						return isolate->InsertToHeap(new Token(AND_NOT_ASSIGN), INTERNAL_TOKEN);
					}
					Next();
					return isolate->InsertToHeap(new Token(AND_NOT), INTERNAL_TOKEN);
				}
				else if (p == '=') {
					Next();
					return isolate->InsertToHeap(new Token(AND_ASSIGN), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(AND), INTERNAL_TOKEN);
			}
			case '|': {
				char p = Peek();
				if (p == '|') {
					Next(); 
					return isolate->InsertToHeap(new Token(LOR), INTERNAL_TOKEN);
				}
				else if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(OR_ASSIGN), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(OR), INTERNAL_TOKEN);
			}
			case '^': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(XOR_ASSIGN), INTERNAL_TOKEN);
				}
			}
			case '=': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return isolate->InsertToHeap(new Token(EQL), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(ASSIGN), INTERNAL_TOKEN);
			}
			case ';': return isolate->InsertToHeap(new Token(SEMICOLON), INTERNAL_TOKEN);
			case ':': return isolate->InsertToHeap(new Token(COLON), INTERNAL_TOKEN);
			case ',': return isolate->InsertToHeap(new Token(COMMA), INTERNAL_TOKEN);
			case '.': {
				char p = Peek();
				char p2 = src->at(readOffset + 1);
				if (p == '.' && p2 == '.') {
					Next();
					Next();
					return isolate->InsertToHeap(new Token(ELLIPSIS), INTERNAL_TOKEN);
				}
				return isolate->InsertToHeap(new Token(PERIOD), INTERNAL_TOKEN);
			}
			case '(': return isolate->InsertToHeap(new Token(LPAREN), INTERNAL_TOKEN);
			case ')': return isolate->InsertToHeap(new Token(RPAREN), INTERNAL_TOKEN);;
			case '[': return isolate->InsertToHeap(new Token(LBRACK), INTERNAL_TOKEN);
			case ']': return isolate->InsertToHeap(new Token(RBRACK), INTERNAL_TOKEN);
			case '{': return isolate->InsertToHeap(new Token(LBRACE), INTERNAL_TOKEN);
			case '}': return isolate->InsertToHeap(new Token(RBRACE), INTERNAL_TOKEN);
			case '#': return isolate->InsertToHeap(new Token(HASH), INTERNAL_TOKEN);
			case '\'': {
				char p = Peek();
				char p2 = src->at(readOffset + 1);
				if (p2 == '\''){
					Token* tok = isolate->InsertToHeap(new Token(CHAR), INTERNAL_TOKEN);
					tok->raw = p;
					Next();
					Next();
					return tok;
				}
				else if (p == '\''){
					Next();
					return isolate->InsertToHeap(new Token(CHAR), INTERNAL_TOKEN);
				}
				throw Error::INVALID_CHAR_VALUE;
			}
			case '"': {
				Next(); // eat "
				if (ch == '"') result = "";
				else{
					result = ch;
					while (true){
						Next();
						if (ch == '"' || ch == -1) break;
						result += ch;
					}
				}
				Token* tok = isolate->InsertToHeap(new Token(STRING), INTERNAL_TOKEN);
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
					Token* tok = Token::GetToken(result);
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
					if (isInt) tok = isolate->InsertToHeap(new Token(INT), INTERNAL_TOKEN);
					else if (count < 7) tok = isolate->InsertToHeap(new Token(FLOAT), INTERNAL_TOKEN);
					else tok = isolate->InsertToHeap(new Token(DOUBLE), INTERNAL_TOKEN);
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
		if (ch == ' ' || ch == '\t' || ch == '\n'){
			char p = Peek();
			Next();
			while (true){			
				if (p != ' ' || p != '\t' || p != '\n' || p != -1)
					break;
				p = Peek();
				Next();
			}
		}
	}
} // namespace internal
} // namespace Cobra