#include "parser.h"
#include "cobra/flags.h"

namespace Cobra{
namespace internal{

	Parser* Parser::New(Isolate* isolate, std::string* source){
		void* pt = isolate->GetMemory(sizeof(Parser));
		Parser* p = new(pt) Parser(isolate);
		p->SetDefaults(source);
		return p;
	}

	Parser::Parser(Isolate* iso){
		isolate = iso;
		expected = NULL;
		scanner = NULL;
		tok = NULL;
		pos = -1;
		row = -1;
		col = -1;
		reset = false;
		nonOp = false;
		isInline = false;
		isInternal = false;
		printVariables = PRINT_VARIABLES;
		trace = TRACE_PARSER;
	}

	void Parser::SetDefaults(std::string* source){
		source = source;
		scanner = Scanner::New(isolate, source);
	}

	void Parser::Parse(){
		try{
			Trace("\n\nParsing", "Started");
			ParseImportOrInclude();
			ParseShallowStmtList();
		}
		catch (Error::ERROR e){
			throw e;
		}
	}

	void Parser::Trace(const char* name, const char* value){
		if (trace) printf("%s - %s\n", name, value);
	}

	void Parser::PrintTok(){
		printf("Debug: Raw: %s, Type: %s\n", tok->raw.c_str(), Token::ToString(tok->value));
	}

	void Parser::Next(){
		try {
			Row = row;
			Col = col;
			Pos = pos;
			if (tok != NULL) isolate->FreeMemory(tok, sizeof(Token));
			tok = scanner->NextToken();
			row = scanner->row;
			col = scanner->col;
			pos = scanner->offset;
		}
		catch (Error::ERROR e){
			throw e;
		}
	}

	void Parser::Expect(TOKEN val){
		if (tok == NULL || tok->value != val) {
			Token* t = Token::New(isolate, val);
			expected = t;
			throw Error::EXPECTED;
		}
	}

	bool Parser::Is(int argc, ...){
		va_list ap;
    va_start(ap, argc);
    bool isInList = false;
    for(int i = 1; i <= argc; i++) {
      if (!isInList) isInList = tok->value == va_arg(ap, int);
    }
    va_end(ap);
    return isInList;
	}

	/**
	 * Allowed syntax for include || import:
	 * 
	 * 		import "string"
	 *   	import "string" as "s"
	 *    import {
	 * 		 	"string",
	 * 		  "exception" as "e"
	 *    }
	 */
	void Parser::ParseImportOrInclude(){
		Next(); // first token
		bool isImport = false;
		bool group = false;
		while (true){
			isImport = Is(1, IMPORT);
			if (Is(2, IMPORT, INCLUDE)) {
				Next();
				if (Is(1, LBRACE)) {
					group = true;
					Next(); // eat {
				}
				else if (!Is(1, STRING)) throw Error::INVALID_INCLUDE_IMPORT;
			}
			else if (group && Is(1, COMMA)) Next(); // Eat ,
			else break;
			if (isImport) {
				ASTImport* import = ASTImport::New(isolate);
				import->name = tok->raw;
				import->alias = ParseAlias();
				imports.push_back(import);
			}
			else {
				ASTInclude* include = ASTInclude::New(isolate);
				include->name = tok->raw;
				include->alias = ParseAlias();
				includes.push_back(include);
			}
			if (group && !Is(2, COMMA, RBRACE)) throw Error::INVALID_INCLUDE_IMPORT;
		}
	}

	/**
	 * Allowed syntax:
	 * 
	 * 		as "string"
	 */
	std::string Parser::ParseAlias(){
		Next();
		if (Is(1, AS)){
			Next();
			Expect(STRING);
			std::string result = tok->raw;
			Next();
			return result;
		}
		return "";
	}

	void Parser::ParseShallowStmtList(){
		int type = tok->Int();
		switch (type){
			case FUNC: ParseFunc(); break;
		}
	}

	void Parser::ParseFunc(){
		Trace("Parsing", "Func");
		Expect(FUNC);
		Next();
		Expect(IDENT);
		ASTFunc* func = ASTFunc::New(isolate);
		func->name = tok->raw;
		Next();
		Expect(LPAREN);
		while (true){
			if (!Is(3, IDENT, RPAREN)) throw Error::INVALID_ARGUMENT_TYPE;
			if (Is(1, IDENT)){
				std::string first = tok->raw;
				Next();
				std::string second = "";
				if (Is(1, IDENT)) {
					second = tok->raw;
					Next();
				}
				ASTVar* var = ASTVar::New(isolate);
				var->name = first;
				func->args.push_back(var);
			}
			if (Is(1, RPAREN)) break;
			Expect(COMMA);
			Next();
		}
		Next(); // eat )
	}
} // namespace internal
}	// namespace Cobra