#include "parser.h"

Parser::Parser(std::string* raw){
	kRaw = raw;
}

Parser::~Parser(){}

void Parser::Parse(){
	Syntax s(kRaw);
	try {
		s.Check();
	}
	catch (Error::Value v){
		printf("Error: %s\n", Error::GetName(v));
	}
}