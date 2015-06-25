#include "scanner.h"

namespace Cobra{
	Scanner::Scanner(std::string source){
		src = source;
		offset = -1;
		readOffset = 0;
		row = 1;
	}

	Token* Scanner::NextToken(){
		Next(); // set char

		switch (ch){
			case -1: return NULL;
			case '\t': case ' ': {
				ScanWhiteSpaces();
				return NextToken();
			}
			case '/': {
				char p = Peek();
				if (p == '*'){
					ScanComments(true);
					Next();
					return NextToken();
				}
				else if (p == '/'){
					ScanComments(false);
					Next();
					return NextToken();
				}
				else if (p == '='){
					return new Token(DIV_ASSIGN);
				}
				else {
					return new Token(DIV);
				}
			}
			case '*': {
				char p = Peek();
				if (p == '=') return new Token(MUL_ASSIGN);
				return new Token(MUL);
			}
			case '+': {
				char p = Peek();
				if (p == '+') return new Token(INC);
				else if (p == '=') return new Token(ADD_ASSIGN);
				else return new Token(ADD);
			}
			case '-': {
				char p = Peek();
				if (p == '=') return new Token(SUB_ASSIGN);
				else if (p == '-') return new Token(DEC);
				else if (p == '>') return new Token(RARROW);
				else return new Token(SUB);
			}
			case '%': {
				char p = Peek();
				if (p == '=') return new Token(MOD_ASSIGN);
				return new Token(MOD);
			}
			case '<': {
				char p = Peek();
				if (p == '-') return new Token(LARROW);
				else if (p == '<') {
					char p2 = src[readOffset + 1];
					if (p2 == '=') return new Token(SHL_ASSIGN);
					return new Token(SHL);
				}
				else if (p == '=') return new Token(LEQ);
				else return new Token(LSS);
			}
			case '>': {
				char p = Peek();
				if (p == '=') return new Token(GEQ);
				if (p == '>') {
					char p2 = src[readOffset + 1];
					if (p2 == '=') return new Token(SHR_ASSIGN);
					return new Token(SHR); 
				}
				else return new Token(GTR);
			}
			case '&': {
				char p = Peek();
				if (p == '&') return new Token(LAND);
				else if (p == '^') {
					char p2 = src[readOffset + 1];
					if (p2 == '=') return new Token(AND_NOT_ASSIGN);
					return new Token(AND_NOT);
				}
				else if (p == '=') return new Token(AND_ASSIGN);
				return new Token(AND);
			}
			case '|': {
				char p = Peek();
				if (p == '|') return new Token(LOR);
				else if (p == '=') return new Token(OR_ASSIGN);
				return new Token(OR);
			}
			case '^': {
				char p = Peek();
				if (p == '=') return new Token(XOR_ASSIGN);
			}
			case '=': {
				char p = Peek();
				if (p == '=') return new Token(EQL);
				return new Token(ASSIGN);
			}
			case ';': return new Token(SEMICOLON);
			case ':': return new Token(COLON);
			case ',': return new Token(COMMA);
			case '.': {
				char p = Peek();
				char p2 = src[readOffset + 1];
				if (p == '.' && p2 == '.') return new Token(ELLIPSIS);
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
				char p2 = src[readOffset + 1];
				if (p2 == '\''){
					Token* tok = new Token(CHAR);
					tok->raw = p;
					return tok;
				}
				else if (p == '\''){
					return new Token(CHAR);
				}
				Error("Invalid char value");
			}
			case '"': {
				Next();
				result = ch;
				while (ch != '"'){
					Next();
					result += ch;
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

	void Scanner::Error(std::string err){
		err += row;
		err += ":";
		err += offset;
		errors.push_back(err);
	}

	bool Scanner::IsLetter(char letter){
		if (letter != -1) return isalpha(letter);
		return isalpha(ch);
	}

	bool Scanner::IsNumber(char letter){
		if (letter != -1) return isdigit(letter);
		return isdigit(ch);
	}

	void Scanner::Next(){
		if (readOffset < src.length()){
			offset = readOffset;
			if (ch == '\n') {
				row++;
				col = 0;
				ch = src[offset];
				readOffset++;
				Next();
			}
			else{
				ch = src[offset];
				if (ch == 0) Error("Illegal null");
				readOffset++;
				col++;
			}
		}
		else{
			offset = src.length();
			if (ch == '\n') row++;
			ch = -1; // EOF
		}
	}

	char Scanner::Peek(){
		if (readOffset < src.length()){
			return src[readOffset];
		}
		return -1; // EOF
	}

	void Scanner::ScanComments(bool star){
		Next(); // eat the second comment char
		char p = Peek();
		if (star){
			while (!(ch == '*' && p == '/') && ch != -1){
				Next();
				p = Peek();
			}
		}
		else{
			while (p != '\n'){
				Next();
				p = Peek();
			}
			Next(); // eat \n
		}
		ScanWhiteSpaces();
	}

	void Scanner::ScanWhiteSpaces(){
		char p = Peek();
		while (p == ' ' || p == '\t'){
			Next();
			p = Peek();
		}
	}

} // namespace Cobra