#include "lexer.h"

namespace Cobra{
	Lexer::Lexer(std::string* code){
		mRaw = code;
		mPos = 0;
		mCol = 1;
		mRow = 1;
		mCurrTok = new Tokens::Token;
	}

	Lexer::~Lexer(){
		delete mCurrTok;
	}

	char Lexer::nextChar(){
		if (mRaw->length() > mPos){
			char p = peek();
			if (p == '\n'){
				mRow++;
				mCol = 0;
			}
			mCol++;
			mCh = mRaw->at(mPos++);
			return mCh;
		}
		return -1;
	}

	char Lexer::peek(){
		if (mRaw->length() > mPos){
			return mRaw->at(mPos);
		}
		return -1;
	}

	void Lexer::skipSpace(){
		char p = peek();
		if (p == ' '){
			while (p == ' ' && p != -1){
				nextChar();
				p = peek();
			}
		}
		if (p == -1)
			mCh = -1;
	}

	void Lexer::scanLetters(){
		int startPos = mPos;
		nextChar();
		char p = peek();
		while (isalnum(p) || p == '_'){
			nextChar();
			p = peek();
		}
		int endPos = mPos;
		mBuffer = mRaw->substr(startPos, (endPos - startPos));
	}

	void Lexer::scanNumbers(){
		int startPos = mPos;
		nextChar();
		char p = peek();
		while (isdigit(p) || p == '.'){
			nextChar();
			p = peek();
		}
		int endPos = mPos;
		mBuffer = mRaw->substr(startPos, (endPos - startPos));
	}

	void Lexer::bufferToToken(){
		mCurrTok->pos = &mPos;
		mCurrTok->col = &mCol;
		mCurrTok->row = &mRow;
		if (mBuffer.length() < 1){
			mCurrTok->raw = NULL;
		}
		else{
			mCurrTok->raw = mBuffer.c_str();
		}
	}

	const char* Lexer::getTokenText(){
		return mCurrTok->raw;
	}

	void Lexer::nextToken(){
		mBuffer = "";
		skipSpace();
		char p = peek();
		switch (p){
			case '<':
			case '=':
			case '>':
			case '!':
			case '&':
			case '|':
			case '%':
			case '+':
			case '*':
			case '/':
			case '-':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case '^':
			case '#':
			case '@':
			case '$':
			case '~': 
			case '.':
			case ';':
			case ':':
			case '\'':
			case '"':
			case '\\': {
				nextChar();
				mBuffer = mCh;
				return bufferToToken();	
			}
		}
		if (p == '_' || isalpha(p)){
			scanLetters();
			return bufferToToken();
		}
		else if (isdigit(p)){
			scanNumbers();
			return bufferToToken();
		}
		else if (p == -1){
			return bufferToToken();
		}
		else{
			return bufferToToken();
		}
	}
}