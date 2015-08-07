#include "parser.h"

// Search for TODO
// Search remove to remove stmts

namespace Cobra{
namespace internal{

	Parser::Parser(std::string* source){
		scanner = new Scanner(source);
		topScope = new Scope;
		expected = NULL;
		currentFunctionScope = NULL;
		trace = false;
		printVariables = false;
		printCheck = false;
		tok = NULL;
	}

	Parser::~Parser(){
		delete scanner;
		delete topScope;
		delete currentFunctionScope;
		delete expected;
		delete tok;
	}

	std::string Parser::GetParserOptions(){
		if (trace) return "trace";
		else if (printVariables) return "printVariables";
		else if (printCheck) return "printCheck";
		else return "";
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
		try {
			delete tok;
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
			Token* t = new Token(val);
			expected = t;
			throw Error::EXPECTED;
		}
	}

	bool Parser::IsOperator(){
		int type = (int) tok->value;
		return type == ADD || type == SUB || type == MOD || type == DIV || type == MUL || type == SHL || type == SHR;
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

	bool Parser::IsBoolean(){
		int type = (int) tok->value;
		return type == LAND || type == LOR || type == EQL || type == LSS || type == GTR
					|| type == NEQ || type == LEQ || type == GEQ || type == NOT || type == XOR || type == AND;
	}

	ASTFile* Parser::Parse(){
		if (trace) Trace("Parsing", "Started");
		ASTFile* file = new ASTFile;

		//Compiler options
		ParseOptions();

		ParseMode(); // parse the file mode
		ParseImportOrInclude();

		ParseStmtList();

		file->scope = topScope;

		if (printVariables){
			printf("\nGlobal\n------------\n");
			file->scope->String();
		}
		return file;
	}

	ASTFor* Parser::ParseFor(){
		if (trace) Trace("Parsing", "For");
		Expect(FOR);
		Next();
		Expect(LPAREN);
		Next();
		ASTFor* forStmt = new ASTFor;
		forStmt->var = ParseVar();
		forStmt->conditions = ParseUnaryExpr();
		Expect(SEMICOLON);
		Next();
		forStmt->iterator = ParseExpr();
		Expect(RPAREN);
		Next();
		forStmt->block = ParseBlock(false);
		return forStmt;
	}

	ASTWhile* Parser::ParseWhile(){
		if (trace) Trace("Parsing", "While Statement");
		Expect(WHILE);
		Next();
		ASTWhile* whileStmt = new ASTWhile;
		whileStmt->conditions = ParseUnaryExpr();
		whileStmt->block = ParseBlock(false);
		return whileStmt;
	}

	ASTElse* Parser::ParseElse(){
		if (trace) Trace("Parsing", "Else Statement");
		Expect(ELSE);
		Next();
		ASTElse* elseStmt = new ASTElse;
		if (tok->value == IF){
			elseStmt->ifStmt = ParseIf();
			elseStmt->conditions = NULL;
			elseStmt->block = NULL;
			return elseStmt;
		}
		elseStmt->conditions = NULL;
		elseStmt->block = ParseBlock(false);
		return elseStmt;

	}

	ASTIf* Parser::ParseIf(){
		if (trace) Trace("Parsing", "If Statement");
		Expect(IF);
		Next();
		ASTExpr* expr = ParseUnaryExpr();
		ASTIf* ifStmt = new ASTIf;
		ifStmt->conditions = expr;
		ifStmt->block = ParseBlock(false);
		return ifStmt;
	}

	VISIBILITY Parser::GetVisibility(){
		int val = (int) tok->value;
		VISIBILITY v;
		switch (val){
			case CONSTRUCTOR: {
				if (trace) Trace("Parsing", "Constructor");
				v = vCONSTRUCTOR;
				Next();
				break;
			}
			case PUBLIC: {
				if (trace) Trace("Parsing", "Public");
				v = vPUBLIC;
				Next();
				break;
			}
			case PRIVATE: {
				if (trace) Trace("Parsing", "Private");
				v = vPRIVATE;
				Next();
				break;
			}
			case STATIC: {
				if (trace) Trace("Parsing", "Static");
				v = vSTATIC;
				Next();
				break;
			}
			default: {
				v = vPUBLIC;
			}
		}
		return v;
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
		ASTNode* node = NULL;
		while (true){
			if (tok->value == RBRACE) break; // empty object
			if (mode == STRICT){
				VISIBILITY v = GetVisibility();
				node = ParseNodes();
				node->visibility = v;
				if (node == NULL) throw Error::EXPECTED_VARIABLE_TYPE;
				obj->members[node->name] = node;
			}
			else{
				VISIBILITY v = GetVisibility();
				node = NULL;
				if (tok->value == FUNC) node = ParseFunc();
				else node = ParseVar();
				node->visibility = v;
				obj->members[node->name] = node;
			}
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
			delete expr;
			return aryMem;
		}
		return expr;
	}

	ASTExpr* Parser::ParseFuncCall(ASTExpr* expr){
		// This will always be a function call
		if (tok->value == LPAREN && expr->type == IDENT){
			if (trace) Trace("Parsing", "Function call");

			ASTFuncCallExpr* call = new ASTFuncCallExpr;
			ASTIdent* ident = (ASTIdent*) expr;
			call->name = ident->name;
			call->pos = ident->pos;
			Next();
			bool expectExpr = false;
			ASTExpr* ex = NULL;
			while (true){
				ex = ParseExpr();
				if (ex != NULL) {
					expectExpr = false;
					call->params.push_back(ex);
				}
				if (tok->value == COMMA){
					Next();
					expectExpr = true;
					continue;
				}
				else if (tok->value == RPAREN){
					if (expectExpr) throw Error::EXPECTED_PARAMETER;
					Next();
					break;
				}
				else{
					throw Error::INVALID_FUNCTION_CALL;
				}
			}
			return call;
		}
		return expr;
	}

	ASTIdent* Parser::ParseIdent(){
		Expect(IDENT);
		if (trace) Trace("Identifier Value", tok->raw.c_str());
		ASTIdent* ident = new ASTIdent;
		ident->pos = pos;
		ident->name = tok->raw;
		Next(); // eat identifier
		return ident;
	}

	ASTExpr* Parser::ParseOperand(){
		if (trace) Trace("Parsing", "Operand");
		int type = (int)tok->value;
		switch (type){
			case IDENT: {
				ASTIdent* first = ParseIdent();
				if (tok->value == PERIOD){ // object.member
					if (trace) Trace("Parsing", "Object member");
					Next();
					ASTExpr* second = ParseOperand();
					if (second->type != IDENT) throw Error::INVALID_OBJECT_MEMBER;
					ASTObjectMemberChainExpr* chain = new ASTObjectMemberChainExpr;
					chain->object = first;
					chain->member = second;
					chain->name = '%' + first->name; // signal to not include in map
					return chain;
				}
				return first;
			}
			case kNULL: case BOOLEAN: case INT: case FLOAT: case DOUBLE: case CHAR: case STRING: {
				ASTLiterary* lit = new ASTLiterary;
				lit->kind = tok->value;
				if (tok->value == kNULL){
					lit->value = "null";
				}
				else{
					lit->value = tok->raw;
				}
				lit->pos = pos;
				Next();
				if (trace) Trace("Literal Value1", lit->value.c_str());
				return lit;
			}
			case LPAREN: {
				return ParseUnaryExpr();
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
			expr = ParseFuncCall(expr);
			expr = ParseArray(expr);
			if (!IsOperator() && !IsBoolean())
				break;
			if (IsBoolean() && trace) Trace("Parsing", "Boolean statement");
			ASTBinaryExpr* binary = new ASTBinaryExpr;
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
			case ADD: case SUB: case MUL: case DIV: case MOD: {
				ASTUnaryExpr* unary = new ASTUnaryExpr;
				unary->pos = pos;
				unary->op = new Token(tok->value);
				delete unary->value;
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
		unary = ParseArray(unary);
		return unary;
	}

	ASTExpr* Parser::ParseExpr(){
		if (trace) Trace("Parsing", "Expression");
		ASTExpr* expr = ParseBinaryExpr();
		if (tok->value == NEW){
			if (trace) Trace("Parsing", "New Object");
			Next(); // eat new
			ASTExpr* e = ParseIdent();
			e = ParseFuncCall(e);
			ASTFuncCallExpr* obj = (ASTFuncCallExpr*) e;
			obj->isNew = true;
			obj->pos = pos;
			return e;
		}
		else if (IsOperator() || IsBoolean()){
			ASTBinaryExpr* binary = new ASTBinaryExpr;
			binary->op = new Token(tok->value);
			Next(); // eat the operator
			binary->Right = ParsePrimaryExpr();
			binary->Left = expr;
			return binary;
		}
		else if (tok->value == DEC){
			ASTIdent* ident = (ASTIdent*) expr;
			ident->dec = true;
			ident->post = true;
			Next();
			return ident;
		}
		else if (tok->value == INC){
			ASTIdent* ident = (ASTIdent*) expr;
			ident->inc = true;
			ident->post = true;
			Next();
			return ident;
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
		ASTVar* var = new ASTVar;
		if (mode == STRICT){
			if (trace) Trace("Parsing", ("Var: " + tok->String()).c_str());
			var->varType = tok->value;
			Next();
		}
		else{
			if (trace) Trace("Parsing", ("Var: " + tok->raw).c_str());
		}
		Expect(IDENT);		
		var->name = tok->raw;		
		var->stmt = ParseSimpleStmt();
		return var;
	}

	ASTNode* Parser::ParseIdentStart(){
		if (trace) Trace("Parsing", "ident start");
		Expect(IDENT);
		ASTNode* expr = ParseExpr(); // here
		if (IsAssignment()){
			if (expr->type == ARRAY_MEMBER){ // for arrays
				ASTArrayMemberExpr* aryMem = (ASTArrayMemberExpr*) expr;
				aryMem->value = ParseSimpleStmt();
			}
			else if (expr->type == OBJECT_MEMBER_CHAIN){
				ASTObjectMemberChainExpr* chain = (ASTObjectMemberChainExpr*) expr;
				chain->value = ParseSimpleStmt();
				chain->isSetting = true;
				Expect(SEMICOLON);
				Next();
				return chain;
			}
		}
		else if (tok->value == SEMICOLON){
			Next();
			return expr;
		}
		else if (expr->type == IDENT){
			ASTVar* var = new ASTVar;
			var->varType = CLASS;
			var->varClass = (ASTIdent*) expr;
			var->stmt = ParseSimpleStmt();
			var->name = tok->raw;
			return var;
		}
		else{
			Expect(SEMICOLON);
			Next();
			return expr;
		}
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
			case OBJECT: return ParseObject();
			case INT: case BOOLEAN: case DOUBLE: case FLOAT: case CHAR: case STRING:{
				if (mode == LAZY)
					throw Error::EXPECTED_VAR;
				return ParseVar();
			}
			case IDENT: return ParseIdentStart();
			case IF: return ParseIf();
			case ELSE: return ParseElse();
			case WHILE: return ParseWhile();
			case FOR: return ParseFor();
			case CONST: {
				//
			}
			case FUNC: return ParseFunc();
			case EXPORT: {
				Next(); // eat export;
				ASTNode* expr = ParseStmt();
				if (trace) Trace("export", expr->name.c_str());
				exports.push_back(expr);
				return expr;
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
			ASTNode* stmt = ParseStmt();
			if (stmt == NULL) break;
			if (topScope == NULL) throw Error::INTERNAL_SCOPE_ERROR;
			topScope->Insert(stmt);
			if (tok->value == END || tok->value == RBRACE) break;
		}
	}

	ASTBlock* Parser::ParseBlock(bool initEat){
		if (trace) Trace("Parsing", "Block");
		if (initEat)
			Next();
		Expect(LBRACE);
		Next();
		OpenScope();

		ParseStmtList();
		Expect(RBRACE);
		Next();

		ASTBlock* block = new ASTBlock;
		block->scope = topScope;

		if (printVariables) topScope->String();

		CloseScope();
		return block;
	}

	void Parser::ParseFuncParams(ASTFunc* func){
		if (trace) Trace("Parsing", "Func Parameters");
		ASTVar* var = NULL;
		while (true){
			if (mode == STRICT){
				if (tok->value == RPAREN) return;
				if (!IsVarType()) throw Error::EXPECTED_VARIABLE_TYPE;
				TOKEN rType = tok->value;
				Next();
				Expect(IDENT);
				var = new ASTVar;
				std::string name = tok->raw;
				var->varType = rType;
				Next();
				if (tok->value == LBRACK){
					Next(); // eat [
					Expect(RBRACK);
					Next(); // eat ]
					var->arrayType = rType;
					var->varType = ARRAY;
				}
				if (var == NULL) throw Error::EXPECTED_VARIABLE_TYPE;
				var->name = name;

				if (tok->value == COMMA) {
					func->args[name] = var;
					func->ordered.push_back(var);
					Next();
				}
				else if (tok->value == RPAREN) {
					func->args[name] = var;
					func->ordered.push_back(var);
					break;
				}
				else Expect(RPAREN);
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
					delete var;
					var = new ASTArray(VAR);
					var->name = name;
				}
				else{
					var->name = name;
				}

				if (tok->value == COMMA) {
					func->args[name] = var;
					func->ordered.push_back(var);
					Next();
				}
				else if (tok->value == RPAREN) {
					func->args[name] = var;
					func->ordered.push_back(var);
					break;
				}
				else Expect(RPAREN);
			}
		}
	}

	ASTFunc* Parser::ParseFunc(){
		Next(); // eat func
		Expect(IDENT);
		ASTFunc* func = new ASTFunc;
		func->name = tok->raw; // set the func name
		if (trace) {
			std::string str = "Func: ";
			str += func->name + "()";
			Trace("Parsing", str.c_str());
		}
		Next();
		Expect(LPAREN);
		Next();
		ParseFuncParams(func);
		Expect(RPAREN);

		if (printVariables) printf("\n%s\n------------\n", func->name.c_str());

		func->body = ParseBlock(true);

		// add the parameters to the first func scope
		// for (int i = 0; i < func->ordered.size(); i++){
		// 	func->body->scope->InsertBefore(func->ordered[i]);
		// }

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
			case OBJECT: return ParseObject();
			case INT: case BOOLEAN: case DOUBLE: case FLOAT: case CHAR: case STRING: {
				if (mode == LAZY) throw Error::EXPECTED_VAR;
				return ParseVar();
			}
			case IDENT: return ParseIdentStart();
			case IF: return ParseIf();
			case ELSE: return ParseElse();
			case WHILE: return ParseWhile();
			case FOR: return ParseFor();
			case RETURN: throw Error::UNEXPECTED_RETURN;
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
					if (isImport) {
						ASTImport* import = new ASTImport;
						import->name = tok->raw;
						Next();
						if (tok->value == AS){
							Next(); // eat as
							Expect(IDENT);
							import->alias = tok->raw;
							Next();
							Expect(SEMICOLON);
						}
						else{
							Expect(SEMICOLON);
						}
						imports.push_back(import);
					}
					else {
						ASTInclude* include = new ASTInclude;
						include->name = tok->raw;
						Next();
						if (tok->value == AS){
							Next();
							Expect(IDENT);
							include->alias = tok->raw;
							Next();
							Expect(SEMICOLON);
						}
						includes.push_back(include);
					}
					Next();
					Expect(SEMICOLON);
					Next();
					if (tok->value == RBRACE) break;
				}
			}
			else{
				Expect(STRING);
				if (isImport){
					ASTImport* import = new ASTImport;
					import->name = tok->raw;
					Next();
					if (tok->value == AS){
						Next();
						Expect(IDENT);
						import->alias = tok->raw;
						Next();
						Expect(SEMICOLON);
					}
					imports.push_back(import);
				}
				else {
					ASTInclude* include = new ASTInclude;
					include->name = tok->raw;
					Next();
					if (tok->value == AS){
						Next();
						Expect(IDENT);
						include->alias = tok->raw;
						Next();
						Expect(SEMICOLON);
					}
					includes.push_back(include);
				}
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

	/**
	 * These options are internal
	 *
	 * %trace; - traces the parsing method by method
	 * %printVariables; - prints all the variables in a given scope
	 */
	void Parser::ParseOptions(){
		Next();
		if (tok->value == MOD){
			Next();
			Expect(IDENT);
			if (tok->raw == "trace")
				trace = true;
			else if (tok->raw == "printVariables")
				printVariables = true;
			else if (tok->raw == "printCheck")
				printCheck = true;
			Next();
			Next(); // eat ;
		}
	}
} // namespace internal
}	// namespace Cobra