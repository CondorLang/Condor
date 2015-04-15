#include "lexer.h"

using namespace Cobra;

Lexer::Lexer(std::string code){
	rawCode = code;
	pos = 0;
	col = 0;
	row = 1;
	rawCodeLength = rawCode.length();
	hasError = false;
}

Lexer::~Lexer(){}

char Lexer::peek(){
	if (pos < rawCodeLength){
		char character = rawCode[pos];
		return character;
	}
	else{
		return -1; // end of file
	}
}

char Lexer::nextChar(){
	if (pos < rawCodeLength){
		char character = rawCode[pos];
		if (character == '\r') character = rawCode[++pos]; // eat the \r
		if (character == '\n') {
			col = 0;
			pos++;
			row++;
			return nextChar();
		}
		else {
			pos++;
			col++;
			return character;
		}
	}
	else{
		return -1; // end of file
	}
}

void Lexer::eatSpaces(){
	char p = peek();
	while (p == ' '){
		nextChar();
		p = peek();
	}
}

void Lexer::eatComments(){
	char p = peek();
	if (p == '/'){
		char currentChar = nextChar();
		while (currentChar != '\n'){
			currentChar = nextChar();
		}
	}
	else if (p == '*'){
		char currentChar = nextChar();
		p = peek();
		while (currentChar != '*' && p != '/'){
			currentChar = nextChar();
			p = peek();
		}
	}
}

void Lexer::nextToken(Tokens::Token* tok){
	char currentChar = nextChar();
	switch (currentChar) {
		case ' ': {
			eatSpaces(); // purposely don't break;
			nextToken(tok);
			return;
		}
		case '/': {
			char p = peek();
			if (p == '/' || p == '*'){
				eatComments();
			}
			else{
				tok->type = Tokens::OPERATOR;
				tok->lexeme = "/";
				return;
			}
		}
		case '{': case '}':
		case '(': case ')':
		case '[': case ']': {
			tok->type = Tokens::DELIMITER;
			tok->lexeme = currentChar;
			return;
		}
		case ':': case ';': 
		case ',': {
			tok->type = Tokens::PUNCTUATOR;
			tok->lexeme = currentChar;
			return;
		}
		case '=': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = "==";
			}
			else{
				tok->lexeme = "=";
			}
			return;
		}
		case '!': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = "!=";
			}
			else{
				tok->lexeme = "!";
			}
			return;
		}
		case '<': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = "<=";
			}
			else{
				tok->lexeme = "<";
			}
			return;
		}
		case '>': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = ">=";
			}
			else{
				tok->lexeme = ">";
			}
			return;
		}
		case '+': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = "+=";
			}
			else if (p == '+'){
				nextChar();
				tok->lexeme = "++";
			}
			else{
				tok->lexeme = "+";
			}
			return;
		}
		case '-': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = "-=";
			}
			else if (p == '-'){
				nextChar();
				tok->lexeme = "--";
			}
			else{
				tok->lexeme = "-";
			}
			return;
		}
		case '*': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '='){
				nextChar();
				tok->lexeme = "*=";
			}
			else{
				tok->lexeme = "*";
			}
			return;
		}
		case '|': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '|'){
				nextChar();
				tok->lexeme = "||";
			}
			else{
				tok->lexeme = "|";
			}
			return;
		}
		case '&': {
			char p = peek();
			tok->type = Tokens::OPERATOR;
			if (p == '&'){
				nextChar();
				tok->lexeme = "&&";
			}
			else{
				tok->lexeme = "&";
			}
			return;
		}
	}

	if (isalpha(currentChar)){ // [a-zA-Z]

		std::string identifier;
		identifier += currentChar;
		char p = peek();

		while (isalnum(p)){
			currentChar = nextChar();
			identifier += currentChar;
			p = peek();
		}

		if (identifier == "if"    || identifier == "else"    ||
			  identifier == "func"  || identifier == "for"     ||
			  identifier == "while" || identifier == "each" ||
			  identifier == "var"   || identifier == "do"      ||
			  identifier == "return"){
			tok->type = Tokens::KEYWORD;
			tok->lexeme = identifier;
		}
		else {
			tok->type = Tokens::IDENTIFIER;
			tok->lexeme = identifier;
		}
	}
	else if (isdigit(currentChar) || currentChar == '.'){ // [0-9.]
		std::string constant;
		constant += currentChar;
		char p = peek();

		while (isdigit(p) || p == '.'){
			currentChar = nextChar();
			constant += currentChar;
			p = peek();
		}

		tok->type = Tokens::CONSTANT;
		tok->lexeme = constant;
	}
	else{
		tok->type = Tokens::END;
		tok->lexeme = "";
	}
}