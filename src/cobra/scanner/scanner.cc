#include "scanner.h"

namespace Cobra{
namespace internal{

	/**
	 * @brief Scanner constructor
	 * 
	 * @param source std::string
	 */
	Scanner::Scanner(std::string* source){
		src = source;
		offset = -1;
		readOffset = 0;
		row = 1;
		col = 0;
		ch = -1;
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
			case -1: return new Token(END);
			case '\0': return new Token(END);
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
					Next(); return new Token(DIV_ASSIGN);
				}
				else {
					Next(); return new Token(DIV);
				}
			}
			case '@': {
				return new Token(CONSTRUCTOR);
			}
			case '*': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return new Token(MUL_ASSIGN);
				}
				return new Token(MUL);
			}
			case '+': {
				char p = Peek();
				if (p == '+') {
					Next(); 
					return new Token(INC);
				}
				else if (p == '=') {
					Next(); 
					return new Token(ADD_ASSIGN);
				}
				else return new Token(ADD);
			}
			case '-': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return new Token(SUB_ASSIGN);
				}
				else if (p == '-') {
					Next(); 
					return new Token(DEC);
				}
				else if (p == '>') {
					Next(); 
					return new Token(RARROW);
				}
				else return new Token(SUB);
			}
			case '%': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return new Token(MOD_ASSIGN);
				}
				return new Token(MOD);
			}
			case '<': {
				char p = Peek();
				if (p == '-') {
					Next(); 
					return new Token(LARROW);
				}
				else if (p == '<') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						Next(); 
						return new Token(SHL_ASSIGN);
					}
					Next();
					return new Token(SHL);
				}
				else if (p == '=') {
					Next(); 
					return new Token(LEQ);
				}
				else return new Token(LSS);
			}
			case '>': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return new Token(GEQ);
				}
				if (p == '>') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						Next(); 
						return new Token(SHR_ASSIGN);
					}
					Next();
					return new Token(SHR); 
				}
				else return new Token(GTR);
			}
			case '!': {
				char p = Peek();
				if (p == '='){
					Next();
					return new Token(NEQ);
				}
				return new Token(NOT);
			}
			case '&': {
				char p = Peek();
				if (p == '&') {
					Next(); 
					return new Token(LAND);
				}
				else if (p == '^') {
					char p2 = src->at(readOffset + 1);
					if (p2 == '=') {
						Next(); 
						return new Token(AND_NOT_ASSIGN);
					}
					Next();
					return new Token(AND_NOT);
				}
				else if (p == '=') {
					Next();
					return new Token(AND_ASSIGN);
				}
				return new Token(AND);
			}
			case '|': {
				char p = Peek();
				if (p == '|') {
					Next(); 
					return new Token(LOR);
				}
				else if (p == '=') {
					Next(); 
					return new Token(OR_ASSIGN);
				}
				return new Token(OR);
			}
			case '^': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return new Token(XOR_ASSIGN);
				}
			}
			case '=': {
				char p = Peek();
				if (p == '=') {
					Next(); 
					return new Token(EQL);
				}
				return new Token(ASSIGN);
			}
			case ';': return new Token(SEMICOLON);
			case ':': return new Token(COLON);
			case ',': return new Token(COMMA);
			case '.': {
				char p = Peek();
				char p2 = src->at(readOffset + 1);
				if (p == '.' && p2 == '.') {
					Next();
					Next();
					return new Token(ELLIPSIS);
				}
				return new Token(PERIOD);
			}
			case '(': return new Token(LPAREN);
			case ')': return new Token(RPAREN);
			case '[': return new Token(LBRACK);
			case ']': return new Token(RBRACK);
			case '{': return new Token(LBRACE);
			case '}': return new Token(RBRACE);
			case '#': return new Token(HASH);
			case '\'': {
				char p = Peek();
				char p2 = src->at(readOffset + 1);
				if (p2 == '\''){
					Token* tok = new Token(CHAR);
					tok->raw = p;
					Next();
					Next();
					return tok;
				}
				else if (p == '\''){
					Next();
					return new Token(CHAR);
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
				Token* tok = new Token(STRING);
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
					if (isInt) tok = new Token(INT);
					else if (count < 7) tok = new Token(FLOAT);
					else tok = new Token(DOUBLE);
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