#include "lexer.h"

Lexer::Lexer(std::string* raw){
	kRaw = raw;
	kPos = 0;
	kRow = 0;
	kCol = 0;
	kPrevCol = -1;
}

Lexer::~Lexer(){}

void Lexer::Reset(){
	kPos = 0;
	kRow = 0;
	kCol = 0;
	kPrevCol = -1;
}

void Lexer::next(){
	if (kRaw->length() > kPos){
		kCh = kRaw->at(kPos++);
		kPrevCol = kCol;
		kCol++;
		eatWhiteSpaces();
	}
	else{
		kCh = '\0';
	}
}

void Lexer::back(){
	if (kCol == 0){
		kRow--;
		kCol = kPrevCol;
	}
	else kCol--;
	kCh = kRaw->at(--kPos);
}

char Lexer::peek(){
	if (kRaw->length() > kPos){
		return kRaw->at(kPos++);
	}
	else{
		return '\0';
	}
}

void Lexer::eatWhiteSpaces(){
	if (kCh == '\n' || kCh == '\r'){
		kRow++;
		kCol = 0;
		next();
	}
	else if (kCh == '\t') next();
}

void Lexer::eatLineComment(){
	int row = kRow;
	while (kCh != '\0' || row != kRow){
		next();
	}
}

void Lexer::eatMultiLineComment(){
	char p = peek();
	while (kCh != '*' && p != '/' && kCh != '\0'){
		next();
	}
	next(); // eat /
}

Tokens::Value Lexer::Next(){
	lit = "";
	next();
	switch (kCh){
		case ' ': case '\t': {
			eatWhiteSpaces();
			return Next();
		}
		case '!': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::NEQ;
			}
			return Tokens::NOT;
		}
		case '=': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::EQ;
			}
			return Tokens::ASSIGN;
		}
		case '+': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::PLUS_EQ;
			}
			else if (p == '+'){
				next();
				return Tokens::INC;
			}
			return Tokens::PLUS;
		}
		case '-': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::MINUS_EQ;
			}
			else if (p == '-'){
				next();
				return Tokens::DEC;
			}
			return Tokens::MINUS;
		}
		case '*': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::MULT_EQ;
			}
			return Tokens::MULT;
		}
		case '/': {
			char p = peek();
			if (p == '/'){
				eatLineComment();
				return Next();
			}
			else if (p == '*'){
				eatMultiLineComment();
				return Next();
			}
			else if (p == '='){
				next();
				return Tokens::DIV_EQ;
			}
			return Tokens::DIV;
		}
		case '<': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::LTE;
			}
			return Tokens::LT;
		}
		case '>': {
			char p = peek();
			if (p == '='){
				next();
				return Tokens::GTE;
			}
			return Tokens::GT;
		}
		case '&': {
			char p = peek();
			if (p == '&'){
				next();
				return Tokens::AND;
			}
			return Tokens::ILLEGAL;
		}
		case '|': {
			char p = peek();
			if (p == '|'){
				next();
				return Tokens::OR;
			}
			return Tokens::ILLEGAL;
		}
		case '(': return Tokens::LPAREN;
		case ')': return Tokens::RPAREN;
		case '[': return Tokens::LBRACK;
		case ']': return Tokens::RBRACK;
		case '{': return Tokens::LBRACE;
		case '}': return Tokens::RBRACE;
		case ':': return Tokens::COLON;
		case ';': return Tokens::SEMICOLON;
		case '.': return Tokens::PERIOD;
		case ',': return Tokens::COMMA;
		case '\0': return Tokens::EOS;
		default: {
			Tokens::Value v = ScanKeywordsOrIdentifiers();
			if (v == Tokens::NUMBER){
				v = ScanNumbers();
			}
			return v;
		}
	}
}

Tokens::Value Lexer::ScanNumbers(){
	lit = "";
	bool hasPeriod = false;
	while (kCh != '\0'){
		if (isdigit(kCh) || kCh == '.'){
			if (kCh == '.') hasPeriod = true;
			lit += kCh;
			next();
		}
		else break;
	}
	if (lit.length() == 0) return Tokens::ILLEGAL;
	return Tokens::NUMBER;
}

Tokens::Value Lexer::ScanKeywordsOrIdentifiers(){
	bool first = true;
	lit = "";
	while (kCh != '\0'){
		if (first && (isalpha(kCh) || kCh == '_')){
			first = false;
			lit += kCh;
		}
		else if (first){ // number
			return Tokens::NUMBER;
		}
		else if (!isalnum(kCh) && kCh != '_' && kCh != '$'){
			break;
		}
		else{
			lit += kCh;
		}
		next();
	}
	if (lit.length() > 0) back();
	return Tokens::GetToken(lit.c_str());
}