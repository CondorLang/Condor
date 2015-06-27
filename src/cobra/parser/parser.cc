#include "parser.h"

// Search for TODO
// Search remove to remove stmts

namespace Cobra{

	Parser::Parser(std::string source){
		scanner = new Scanner(source);
		topScope = new Scope;
		currentFunctionScope = NULL;
		trace = true;
	}

	Parser::~Parser(){
		delete scanner;
		delete topScope;
		delete currentFunctionScope;
		delete expected;
	}

	void Parser::Trace(const char* name, const char* value){
		printf("%s - %s\n", name, value);
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

	bool Parser::IsOperator(){
		int type = (int) tok->value;
		return type == ADD || type == SUB || type == MOD || type == DIV || type == MUL;
	}

	void Parser::Parse(){
		if (trace) Trace("Parsing", "Started");
		ASTFile* file = new ASTFile;

		ParseMode(); // parse the file mode
		ParseImportOrInclude();

		while (tok != NULL && tok->value != END){
			topScope->Insert(ParseNodes());
		}

		file->scope = topScope;
		file->scope->String();
	}

	ASTExpr* Parser::ParseIdent(){
		Expect(IDENT);
		if (trace) Trace("Identifier Value", tok->raw.c_str());
		ASTIdent* ident = new ASTIdent;
		ident->pos = pos;
		ident->name = tok->raw;
		Next(); // eat identifier
		return ident;
	}

	/**
	 * TODO:
	 * 	Work on
	 */
	ASTExpr* Parser::ParseOperand(){
		if (trace) Trace("Parsing", "Operand");
		int type = (int)tok->value;
		switch (type){
			case IDENT: return ParseIdent();
			case BOOLEAN: case INT: case FLOAT: case DOUBLE: case CHAR: case STRING: {
				ASTLiterary* lit = new ASTLiterary;
				lit->kind = tok->value;
				lit->value = tok->raw;
				lit->pos = pos;
				Next();
				if (trace) Trace("Literal Value", lit->value.c_str());
				return lit;
			}
		}
		return NULL;
	}

	/**
	 * TODO:
	 * 	Work on operand precidence
	 */
	ASTExpr* Parser::ParsePrimaryExpr(){
		if (trace) Trace("Parsing", "Primary Expression");
		ASTExpr* expr = ParseOperand();

		// Will only loop once
		while (true){
			ASTBinaryExpr* binary = new ASTBinaryExpr;
			if (!IsOperator())
				break;
			binary->op = new Token(tok->value);
			binary->Left = expr;
			Next(); // eat operator
			binary->Right = ParseExpr();
			if (binary->Right == NULL)
				throw Error::MISSING_EXPR;
			return binary;
		}

		return expr;
	}

	/**
	 * TODO:
	 * 	Check to see if done
	 */
	ASTExpr* Parser::ParseUnaryExpr(){
		if (trace) Trace("Parsing", "Unary Expression");
		int type = (int) tok->value;
		switch (type){
			case ADD: case SUB: case NOT: case XOR: case AND: {
				ASTUnaryExpr* unary = new ASTUnaryExpr;
				unary->pos = pos;
				unary->op = new Token(tok->value);
				unary->value = ParseUnaryExpr();
				return unary;
			}
			default: {
				return ParsePrimaryExpr();
			}
		}
	}

	/**
	 * TODO:
	 * 	check to see if done
	 */
	ASTExpr* Parser::ParseBinaryExpr(){
		if (trace) Trace("Parsing", "Binary Expression");
		ASTExpr* unary = ParseUnaryExpr();

		// This will always be a function call
		if (tok->value == LPAREN){
			ASTFuncCallExpr* call = new ASTFuncCallExpr; // here
			ASTIdent* ident = (ASTIdent*) unary;
			call->name = ident->name;
			call->pos = ident->pos;
			unary = ident;
			if (trace) Trace("Parsing", "Function call");
			Next();
			while (true){
				ASTExpr* expr = ParseExpr();
				call->params.push_back(expr);
				if (tok->value == RPAREN){
					Next();
					break;
				}
				else if (tok->value == COMMA){
					Next();
					continue;
				}
				else{
					throw Error::INVALID_FUNCTION_CALL;
				}
			}
		}

		return unary;
	}

	ASTExpr* Parser::ParseExpr(){
		if (trace) Trace("Parsing", "Expression");
		return ParseBinaryExpr();
	}

	/**
	 * TODO:
	 * 	Complete
	 */
	ASTExpr* Parser::ParseSimpleStmt(){
		if (trace) Trace("Parsing", "Simple Statement");
		Next();
		int type = (int) tok->value;

		switch (type){
			case ASSIGN: case ADD_ASSIGN: case SUB_ASSIGN: case MUL_ASSIGN: case DIV_ASSIGN: 
			case MOD_ASSIGN: case AND_ASSIGN: case OR_ASSIGN: case XOR_ASSIGN: case SHL_ASSIGN: 
			case SHR_ASSIGN: case AND_NOT_ASSIGN: {

				ASTUnaryExpr* unary = new ASTUnaryExpr;
				unary->op = new Token(tok->value);
				unary->pos = pos;
				Next();
				unary->value = ParseExpr();
				Expect(SEMICOLON);
				Next(); // eat semi
				return unary;
			}
			default: {
				return ParsePrimaryExpr();
			}
		}
	}

	/**
	 * Return statements are stored as VAR
	 */
	ASTNode* Parser::ParseReturn(){
		if (trace) Trace("Parsing", "Return");
		Expect(RETURN);
		ASTVar* var = new ASTVar;
		var->name = "return";
		var->stmt = ParseSimpleStmt();
		Expect(SEMICOLON);
		Next();
		return var;
	}

	ASTNode* Parser::ParseVar(){
		if (trace) Trace("Parsing", tok->String().c_str());
		ASTVar* var = new ASTVar;
		var->varType = tok->value;
		Next();
		Expect(IDENT);		
		var->name = tok->raw;		
		var->stmt = ParseSimpleStmt();
		return var;
	}

	/**
	 * TODO:
	 * 	Complete all the different statmenet types
	 */
	ASTNode* Parser::ParseStmt(){
		if (trace) Trace("Parsing", "Statement");
		int type = (int) tok->value;
		switch (type){
			case VAR: {
				if (mode == STRICT)
					throw Error::VAR_NOT_ALLOWED_IN_STRICT_MODE;
				return ParseVar();
			}
			case INT: case BOOLEAN: case DOUBLE: case FLOAT: case CHAR: case STRING:{
				if (mode == LAZY)
					throw Error::EXPECTED_VAR;
				return ParseVar();
			}
			case CONST: {
				//
			}
			case RETURN: return ParseReturn();
			default: {
				throw Error::INVALID_STMT;
			}
		}
	}

	/**
	 * {
	 * 		** List of statements **
	 * }
	 */
	void Parser::ParseStmtList(){
		if (trace) Trace("Parsing", "Statement List");
		while (tok->value != CASE && tok->value != DEFAULT && tok->value != RBRACE && tok->value != END){
			topScope->Insert(ParseStmt());
		}
	}

	ASTBlock* Parser::ParseBlock(){
		if (trace) Trace("Parsing", "Block");
		Next();
		Expect(LBRACE);
		Next();
		OpenScope();

		ASTBlock* block = new ASTBlock;

		ParseStmtList();

		Expect(RBRACE);
		Next();

		block->scope = topScope;

		CloseScope();
		return block;
	}

	void Parser::ParseFuncParams(ASTFunc* func){
		if (trace) Trace("Parsing", "Func Parameters");
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
			else{
				Expect(IDENT);
				ASTNode* var = new ASTVar;
				std::string name = tok->raw;
				Next();
				if (tok->value == LBRACK){
					Next();
					Expect(RBRACK);
					Next();
					var = new ASTArray(VAR);
					var->name = name;
				}
				else{
					var->name = name;
				}

				if (tok->value == COMMA) Next();
				else if (tok->value == RPAREN) break;
				else Expect(RPAREN);

				func->args[name] = var;
			}
		}
	}

	ASTFunc* Parser::ParseFunc(){
		if (trace) Trace("Parsing", "Func");
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
		if (trace) Trace("Parsing", "Nodes");
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
		if (tok->value == IMPORT || tok->value == INCLUDE){
			bool isImport = tok->value == IMPORT;

			if (trace){
				if (isImport){
					Trace("Parsing", "Imports");
				}
				else{
					Trace("Parsing", "Includes");
				}
			}

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

		if (trace) Trace("Mode", tok->raw.c_str());

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