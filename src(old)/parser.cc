#include "parser.h"

namespace Cobra{
	Parser::Parser(std::string* code, Context* context){
		mLexer = new Lexer(code);
		mContext = context;
	}

	Parser::~Parser(){
		delete mLexer;
	}

	bool Parser::isStringStart(){
		return mTxt[0] == '\'' || mTxt[0] == '"';
	}

	bool Parser::isStringEnd(){
		int size = strlen(mTxt);
		return mTxt[size - 1] == '\'' || mTxt[size - 1] == '"';
	}

	bool Parser::same(const char* txt){
		int size = strlen(txt);
		return strncmp(txt, mTxt, size) == 0;
	}

	void Parser::err(const char* txt){
		expected = txt;
		found = mTxt;
	}

	std::string Parser::cleanString(){
		std::string str(mTxt);
		str.erase(0, 1);
		str.erase(str.length() - 1, 1);
		return str;
	}

	void Parser::next(bool no_eof){
		mLexer->nextToken();
		mTxt = mLexer->getTokenText();
		if (mTxt == NULL && no_eof){
			err("Unexpected end of file");
			throw EOF_ERROR;
		}
	}

	void Parser::Parse(){
		next(false);
		if (mTxt == NULL)
			return;
		try {
			if (Tokens::isReservedWord(mTxt)){
				parseReservedWord();
			}
		}
		catch (int e){
			throw e;
		}
		Parse();
	}

	void Parser::parseReservedWord(){
		mTxt = mLexer->getTokenText();
		try {
			if (same("import")){
				parseImportOrInclude(true);
			}
			else if (same("include")){
				parseImportOrInclude(false);
			}
		}
		catch (int e){
			throw e;
		}
	}

	void Parser::parseImportOrInclude(bool import){
		next(true);

		/**
		 * import {
		 * 	'name';
		 * 	'name';
		 * }
		 *
		 * include {
		 * 	'path';
		 * 	'path';
		 * }
		 */
		if (same("{")){
			while (true){
				next(true); // eat the {
				if (isStringStart()){
					if (import){
						mContext->addImport(mTxt);
					}
					else{
						try {
							std::string cleaned = cleanString();
							Include::include(cleaned.c_str());
							printf("%s%s\n", "Included file ", mTxt);
						}
						catch (int e){
							err(mTxt);
							throw e;
						}
					}

					if (!isStringEnd()){
						err("Expected end of string");
						throw EXPECTED_END_OF_STRING_ERROR;
					}

					next(true);
					if (!same(";")){
						err("Expected ;");
						throw EXPECTED_SEMICOLON_ERROR;
					}
				}
				else if (same("}")){
					break;
				}
				else{
					err("}");
					throw SYNTAX_ERROR;
				}
			}
		}
		/**
		 * import 'name';
		 * include 'path';
		 */
		else if (isStringStart()){
			if (import){
				mContext->addImport(mTxt);
			}
			else{
				try {
					std::string cleaned = cleanString();
					Include::include(cleaned.c_str());
					printf("%s%s\n", "Included file ", mTxt);
				}
				catch (int e){
					err(mTxt);
					throw e;
				}
			}
			if (!isStringEnd()){
				err("Expected end of string");
				throw EXPECTED_END_OF_STRING_ERROR;
			}
			next(true);
			if (!same(";")){
				err("Expected ;");
				throw EXPECTED_SEMICOLON_ERROR;
			}
		}
		else{
			err("{ or string");
			throw SYNTAX_ERROR;	
		}
	}

	int Parser::getRow(){
		return mLexer->getTokenRow();
	}

	int Parser::getCol(){
		return mLexer->getTokenCol();
	}
}

void Compile(){
	std::string code = "import 'os';\n import 'fs'; import {'path'; 'http';} include '/Users/willdech/Documents/dev/CobraLang/js/parser.js';";
	Cobra::Context context;
	Cobra::Parser parser(&code, &context);	
	try {
		parser.Parse();
	}
	catch (int e) {
		switch (e){
			case SYNTAX_ERROR: {
				printf("%d:%d %s, expected %s but found %s\n", parser.getRow(), parser.getCol(), "Syntax error", parser.expected, parser.found);
				break;
			}
			case EXPECTED_END_OF_STRING_ERROR: 
			case EXPECTED_SEMICOLON_ERROR: 
			case EOF_ERROR: {
				printf("%d:%d %s\n", parser.getRow(), parser.getCol(), parser.expected);
				break;
			}
			case FILE_NOT_EXISTS: {
				printf("%s%s%s\n", "File, ", parser.expected, " does not exist");
				break;
			}
			case FILE_UNREADABLE: {
				printf("%s%s%s\n", "File, ", parser.expected, " is unreadable");
				break;
			}
			case FILE_UNKNOWN: {
				printf("%s%s\n", "Unknown file error with file: ", parser.expected);
			}
		}
	}
}