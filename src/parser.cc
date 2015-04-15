#include "parser.h"

using namespace Cobra;

Parser::Parser(std::string rawCode){
	lexer = new Lexer(rawCode);
	row = 0;
	col = 0;
}

Parser::~Parser(){
	delete lexer;
}

void Parser::nextToken(){
	lexer->nextToken(&token);
	row = lexer->row;
	col = lexer->col - 1; // start of the next token
	lexeme = token.lexeme.c_str();
}

void Parser::Parse(){
	if (token.type == Tokens::END){
		return;
	}
	nextToken();
	
	if (token.lexeme == "func"){
		ParseFunction();
	}
	else{
		return;
	}

	Parse();
}

void Parser::ParseFunction(){
	std::string functionName;
	std::string args[Limits::FUNCTION_ARGUMENTS_LIMIT];
	if (token.lexeme == "func"){
		nextToken();

		// The next token should be an identifier
		if (token.type != Tokens::IDENTIFIER){
			throw Errors::EXPECTED_FUNCTION_NAME;
		}
		functionName = token.lexeme;

		// Disreguard left paren
		nextToken();
		if (token.type != Tokens::DELIMITER && token.lexeme != "("){
			throw Errors::EXPECTED_LEFT_PARENTHESE;
		}

		nextToken();
		if (token.type == Tokens::IDENTIFIER){
			int args_position = 0;
			while (token.type == Tokens::IDENTIFIER){
				args[args_position++] = token.lexeme;
				nextToken(); 
				if (token.type == Tokens::PUNCTUATOR && token.lexeme == ","){
					nextToken();
					// do a brief check to verify the next token is an identifier
					if (token.type != Tokens::IDENTIFIER){
						throw Errors::UNEXPECTED_CHARACTER;
					}
					continue;
				}
				else if (token.type == Tokens::DELIMITER && token.lexeme == ")"){
					break;
				}
				else{
					throw Errors::UNEXPECTED_CHARACTER;
				}
			}
		}
		else if (token.type == Tokens::DELIMITER && token.lexeme == ")"){
			// do nothing with the right paren
		}
		else{
			throw Errors::UNEXPECTED_CHARACTER;
		}

		// expected {
		nextToken();
		if (token.type != Tokens::DELIMITER && token.lexeme != "{"){
			throw Errors::EXPECTED_LEFT_BRACE;
		}

		nextToken();

		ParseBlock();

		Function* function = new Function(functionName, args);
		functions.insert(std::pair<std::string, Function*>(functionName, function));
	}
}

void Parser::ParseBlock(){
	while (token.lexeme != "}"){
		nextToken();
	}
}

bool Parser::exists(const char* name){
	return !(functions.find(name) == functions.end());
}

void Compile(){
	Parser* parser = new Parser("func main(args, argv){print(add(1, 2));} func add(a, b){return a + b;} func minus(a,b){return a - b;}");
	try {
		parser->Parse();
	}
	catch (Errors::ERROR_MSG e){
		const char* errMsg = Errors::toString(e);
		printf("%d:%d - %s, found %s\n", parser->row, parser->col, errMsg, parser->lexeme);
	}
	delete parser;
}