#include "parser.h"

// Search for TODO
// Search remove to remove stmts

namespace Cobra{

	Parser::Parser(std::string source){
		scanner = new Scanner(source);
		topScope = new Scope;
		currentFunctionScope = NULL;
		trace = false;
		printVariables = false;
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

	/**
	 * Debuging
	 */
	void Parser::PrintTok(){
		printf("Debug: %s\n", tok->String().c_str());
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

	bool Parser::IsAssignment(){
		int type = (int) tok->value;
		return type == ASSIGN || type == ADD_ASSIGN || type == SUB_ASSIGN || type == MUL_ASSIGN
					|| type == DIV_ASSIGN || type == MOD_ASSIGN || type == AND_ASSIGN 
					|| type == OR_ASSIGN || type == XOR_ASSIGN || type == SHL_ASSIGN
					|| type == SHR_ASSIGN || type == AND_NOT_ASSIGN;
	}

	bool Parser::IsVarType(){
		int type = (int) tok->value;
		return type == INT || type == BOOLEAN || type == FLOAT || type == DOUBLE || type == CHAR || type == STRING;
	}

	ASTFile* Parser::Parse(){
		if (trace) Trace("Parsing", "Started");
		ASTFile* file = new ASTFile;

		//Compiler options
		ParseOptions();

		ParseMode(); // parse the file mode
		ParseImportOrInclude();

		while (tok != NULL && tok->value != END){
			topScope->Insert(ParseNodes());
		}

		file->scope = topScope;

		if (printVariables){
			printf("\nGlobal\n------------\n");
			file->scope->String();
		}
		return file;
	}

	ASTNode* Parser::ParseObject(){
		Expect(OBJECT);
		Next();
		TOKEN varType;
		if (mode == STRICT && IsVarType()){
			varType = tok->value;
			Next();
		}
		Expect(IDENT);
		if (trace) {
			std::string str = tok->raw;
			str = "Object - " + str;
			Trace("Parsing", str.c_str());
		}
		ASTObject* obj = new ASTObject;
		obj->name = tok->raw;
		Next();
		Expect(LBRACE);
		Next();
		while (true){
			if (tok->value == RBRACE) break; // empty object
			if (mode == LAZY)
				obj->members[tok->raw] = ParseSimpleStmt();
			else{
				ASTExpr* var = new ASTExpr;
				var->assignType = varType;
				var->name = tok->raw;
				var->value = ParseSimpleStmt();
				obj->members[tok->raw] = var;
			}
			if (tok->value == RBRACE) break;
			Next();
		}
		Expect(RBRACE);
		Next();
		return obj;
	}

	ASTExpr* Parser::ParseArray(ASTExpr* expr){
		if (tok->value == LBRACK && expr->type == IDENT){
			if (trace) Trace("Parsing", "Array");
			Next();
			ASTArrayMemberExpr* aryMem = new ASTArrayMemberExpr;
			aryMem->name = expr->name;
			aryMem->member = ParseExpr();
			Expect(RBRACK);
			Next();
			return aryMem;
		}
		return expr;
	}

	void Parser::ParseFuncCall(ASTExpr* expr){
		// This will always be a function call
		if (tok->value == LPAREN && expr->type == IDENT){
			if (trace) Trace("Parsing", "Function call");

			ASTFuncCallExpr* call = new ASTFuncCallExpr;
			ASTIdent* ident = (ASTIdent*) expr;
			call->name = ident->name;
			call->pos = ident->pos;
			expr = ident;
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
			// here
			ParseFuncCall(expr);
			expr = ParseArray(expr);
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
			case LPAREN: {
				Next();
				ASTExpr* expr = ParseExpr();
				Expect(RPAREN);
				Next();
				return expr;
			}
			case LBRACK: {
				return new ASTArrayMemberExpr;
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
		ParseFuncCall(unary);
		unary = ParseArray(unary);
		return unary;
	}

	ASTExpr* Parser::ParseExpr(){
		if (trace) Trace("Parsing", "Expression");
		ASTExpr* expr = ParseBinaryExpr();
		if (IsOperator()){
			ASTBinaryExpr* binary = new ASTBinaryExpr;
			binary->op = new Token(tok->value);
			Next(); // eat the operator
			binary->Right = ParsePrimaryExpr();
			binary->Left = expr;
			return binary;
		}
		else {
			return expr;
		}
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
				return ParseExpr();
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

	ASTNode* Parser::ParseIdentStart(){
		if (trace) Trace("Parsing", "ident start");
		Expect(IDENT);
		ASTNode* expr = ParseExpr();
		if (IsAssignment()){
			if (expr->type == ARRAY_MEMBER){ // for arrays
				ASTArrayMemberExpr* aryMem = (ASTArrayMemberExpr*) expr;
				aryMem->value = ParseSimpleStmt();
			}
		}
		Expect(SEMICOLON);
		Next();
		return expr;
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
			case IDENT: {
				return ParseIdentStart();
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

		if (printVariables) topScope->String();

		CloseScope();
		return block;
	}

	void Parser::ParseFuncParams(ASTFunc* func){
		if (trace) Trace("Parsing", "Func Parameters");
		while (true){
			if (mode == STRICT){
				if (tok->value == RPAREN) return;
				int type = (int)tok->value;
				if (!IsVarType()) throw Error::EXPECTED_VARIABLE_TYPE;
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
				if (tok->value == RPAREN) return;
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

		if (printVariables) printf("\n%s\n------------\n", func->name.c_str());

		func->body = ParseBlock();
		return func;
	}

	/**
	 * Globals
	 */
	ASTNode* Parser::ParseNodes(){
		if (trace) Trace("Parsing", "Nodes");
		int type = (int)tok->value;
		switch (type){
			case FUNC: return ParseFunc();
			case VAR: {
				if (mode == STRICT) throw Error::VAR_NOT_ALLOWED_IN_STRICT_MODE;
				return ParseVar();
			}
			case OBJECT: {
				return ParseObject();
			}
			case INT: case BOOLEAN: case DOUBLE: case FLOAT: case CHAR: case STRING: {
				if (mode == LAZY) throw Error::EXPECTED_VAR;
				return ParseVar();
			}
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

	void Parser::ParseOptions(){
		Next();
		if (tok->value == MOD){
			Next();
			Expect(IDENT);
			if (tok->raw == "trace")
				trace = true;
			else if (tok->raw == "printVariables")
				printVariables = true;
			Next();
			Next(); // eat ;
		}
	}

}	// namespace Cobra