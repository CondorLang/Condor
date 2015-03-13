#include "parser.h"

namespace Cobra{
	Parser::Parser(std::string* code){
		mLexer = new Lexer(code);

		mLexer->nextToken();
		while (mLexer->getTokenText() != NULL){
			const char* txt = mLexer->getTokenText();
			if (Tokens::isReservedWord(txt)){
				printf("%s: %s\n", "Reserved word", txt);
			}
			else{
				printf("%s\n", txt);
			}
			mLexer->nextToken();	
		}
	}

	Parser::~Parser(){
		delete mLexer;
	}
}

void Compile(){
	std::string code = "import 'os';var i = 10.10;";
	Cobra::Parser parser(&code);	
}