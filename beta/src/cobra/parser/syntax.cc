#include "syntax.h"

Syntax::Syntax(std::string* raw){
	kRaw = raw;
	kTok = Tokens::ILLEGAL;
}

Syntax::~Syntax(){}

void Syntax::Check(){
	kLexer.kRaw = kRaw;
	kLexer.Reset();
}

void Syntax::print(Tokens::Value tok){
	printf("Token: %s\n", Tokens::GetName(tok));
}

void Syntax::next(){
	kTok = kLexer.Next();
}