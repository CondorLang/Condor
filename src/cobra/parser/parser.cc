#include "parser.h"

namespace Cobra{

	Parser::Parser(std::string source){
		scanner = new Scanner(source);
		topScope = new Scope;
		currentFunctionScope = NULL;
	}

	Parser::~Parser(){
		delete scanner;
		delete topScope;
	}

	void Parser::OpenScope(){
		topScope = topScope->NewScope();
	}

	void Parser::CloseScope(){
		topScope = topScope->outer;
	}

	void Parser::Next(){
		tok = scanner->NextToken();
		row = scanner->row;
		col = scanner->col;
		pos = scanner->offset;
	}

	void Parser::Expect(TOKEN val){
		if (tok->value != val) {
			Token* t = new Token(val);
			expected = t;
			throw Error::EXPECTED;
		}
	}

	void Parser::Parse(){
		ParseMode(); // parse the file mode
	}

	void Parser::ParseMode(){
		Next();
		Expect(HASH);
		Next();
	}

}	// namespace Cobra

void Compile(){
	std::string fileStr = "";
	std::ifstream in("test/test.cb");
	std::string line;
	while(getline(in, line)) {
	  fileStr += line;
	}
	Cobra::Parser* parser = new Cobra::Parser(fileStr);
	try {
		parser->Parse();
	}
	catch (Cobra::Error::ERROR e){
		std::string msg = Cobra::Error::String(e, parser->expected);
		printf("%d:%d - %s\n", parser->row, parser->col, msg.c_str());
	}
	
}