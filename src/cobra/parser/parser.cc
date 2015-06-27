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
		if (tok == NULL || tok->value != val) {
			Token* t = new Token(val);
			expected = t;
			throw Error::EXPECTED;
		}
	}

	void Parser::Parse(){
		ASTFile* file = new ASTFile;

		ParseMode(); // parse the file mode
		ParseImportOrInclude();

		while (tok != NULL){
			topScope->Insert(ParseNodes());
		}

		file->scope = topScope;
	}

	ASTBlock* Parser::ParseBlock(){
		OpenScope();

		ASTBlock* block = new ASTBlock;
		block->scope = topScope;

		CloseScope();
		return block;
	}

	void Parser::ParseFuncParams(ASTFunc* func){
		while (true){
			if (mode == STRICT){
				int type = (int)tok->value;
				TOKEN rType = tok->value;
				Next();
				Expect(IDENT);
				ASTNode* node = NULL;
				std::string name;
				switch (type){
					case INT: {
						node = new ASTInt;						
						break;
					}
					case FLOAT: {
						node = new ASTFloat;
						break;
					}
					case DOUBLE: {
						node = new ASTDouble;
						break;
					}
					case BOOLEAN: {
						node = new ASTBoolean;
						break;
					}
					case CHAR: {
						node = new ASTChar;
						break;
					}
					case STRING: {
						node = new ASTString;
						break;
					}
					case FUNC: {
						node = new ASTFunc;
						break;
					}
					default: throw Error::EXPECTED_VARIABLE_TYPE;
				}
				name = tok->raw;
				Next();
				if (tok->value == LBRACK){
					Next(); // eat [
					Expect(RBRACK);
					Next(); // eat ]
					node = new ASTArray(rType);
				}

				if (tok->value == COMMA) {
					Next();
				}
				else if (tok->value == RPAREN) break;
				else Expect(RPAREN);

				func->args[name] = node;
			}
		}
	}

	ASTFunc* Parser::ParseFunc(){
		Next(); // eat func
		Expect(IDENT);
		ASTFunc* func = new ASTFunc;
		func->name = tok->raw; // set the func name
		Next();
		Expect(LPAREN);
		Next();
		ParseFuncParams(func);
		Expect(RPAREN);
		func->body = ParseBlock();
		return func;
	}

	ASTNode* Parser::ParseNodes(){
		int type = (int)tok->value;
		switch (type){
			case FUNC: return ParseFunc();
		}
		Next();
		return NULL;
	}

	/**
	 * import "math";
	 *
	 * or
	 *
	 * import {
	 * 	"math";
	 * 	"os";
	 * }
	 */
	void Parser::ParseImportOrInclude(){
		Next();
		if (tok == NULL) return;
		if (tok->value == IMPORT || tok->value == INCLUDE){
			bool isImport = tok->value == IMPORT;
			Next();
			// parsing block imports
			if (tok->value == LBRACE){
				Next();
				while (true){
					Expect(STRING);
					if (isImport) imports.push_back(tok->raw);
					Next();
					Expect(SEMICOLON);
					Next();
					if (tok->value == RBRACE) break;
				}
			}
			else{
				Expect(STRING);
				if (isImport) imports.push_back(tok->raw);
				Next();
				Expect(SEMICOLON);
			}
			ParseImportOrInclude();
		}
	}

	/**
	 * #mode "strict"
	 *
	 * or
	 *
	 * #mode "lazy"
	 */
	void Parser::ParseMode(){
		Next();
		Expect(HASH);
		Next();
		Expect(IDENT);
		if (tok->raw != "mode") {throw Error::EXPECTED_MODE;}
		Next();
		Expect(STRING);
		if (tok->raw == "lazy"){
			mode = LAZY;
		}
		else if (tok->raw == "strict"){
			mode = STRICT;
		}
		else{
			throw Error::MODE_UNKNOWN;
		}
		Next();
		Expect(SEMICOLON);
	}

}	// namespace Cobra

void Compile(){
	std::ifstream in("test/test.cb");
	std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	Cobra::Parser* parser = new Cobra::Parser(fileStr);
	try {
		parser->Parse();
	}
	catch (Cobra::Error::ERROR e){
		std::string msg = Cobra::Error::String(e, parser->expected);
		printf("%d:%d - %s\n", parser->row, parser->col, msg.c_str());
	}
	
}