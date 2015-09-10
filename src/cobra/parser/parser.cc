#include "parser.h"
#include "cobra/flags.h"

namespace Cobra{
namespace internal{

	Parser::Parser(std::string* src, std::string* path){
		scanner = NULL;
		source = src;
		topScope = NULL;
		expected = NULL;
		currentFunctionScope = NULL;
		trace = TRACE_PARSER;
		printVariables = PRINT_VARIABLES;
		printCheck = false;
		tok = NULL;
		isolate = NULL;
		filePath = path;
		Pos = -1;
		Col = -1;
		Row = -1;
		col = -1;
		row = -1;
		pos = -1;
		reset = false;
		internal = false;
		nonOp = false;
	}

	Parser::~Parser(){
		//delete tok;
	}

	ASTFile* Parser::Parse(){
		iHandle<Scanner> scan = isolate->NewHandle(new Scanner(isolate, source), SCANNER);
		scanner = &scan;
		topScope = isolate->InsertToHeap(new Scope, SCOPE);
		if (trace) Trace("Parsing", "Started");
		ASTFile* file = isolate->InsertToHeap(new ASTFile, ASTFILE);

		ParseMode(); // parse the file mode
		if (reset){
			scan = isolate->NewHandle(new Scanner(isolate, source), SCANNER);
			scanner = &scan;
		}
		ParseImportOrInclude();

		ParseStmtList();

		// Set Includes
		for (int i = 0; i < includes.size(); i++){
			ASTInclude* include = includes[i];
			if (include->alias.empty()){
				file->includesList.push_back(include);
			}
			else{
				file->includes[include->alias] = include;
			}
		}

		file->scope = topScope;

		if (printVariables){
			printf("\nGlobal\n------------\n");
			file->scope->String();
		}
		return file;
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
		topScope = isolate->InsertToHeap(topScope->NewScope(), SCOPE);
	}

	void Parser::CloseScope(){
		topScope = topScope->outer;
	}

	void Parser::Next(){
		try {
			iHandle<Scanner> scan = scanner->Localize();
			Row = row;
			Col = col;
			Pos = pos;
			tok = scan->NextToken();
			row = scan->row;
			col = scan->col;
			pos = scan->offset;
		}
		catch (Error::ERROR e){
			throw e;
		}
	}

	void Parser::Expect(TOKEN val){
		if (tok == NULL || tok->value != val) {
			Token* t = isolate->InsertToHeap(new Token(val), K_TOKEN);
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

	void Parser::AddRowCol(ASTNode* node){
		node->row = row;
		node->col = col;
	}

	ASTFor* Parser::ParseFor(){
		if (trace) Trace("Parsing", "For");
		Expect(FOR);
		int r = row;
		int c = col;
		Next();
		Expect(LPAREN);
		Next();
		ASTFor* forStmt = isolate->InsertToHeap(new ASTFor, ASTFOR);
		forStmt->row = r;
		forStmt->col = c;
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
		int r = row;
		int c = col;
		Next();
		ASTWhile* whileStmt = isolate->InsertToHeap(new ASTWhile, ASTWHILE);
		whileStmt->row = r;
		whileStmt->col = c;
		whileStmt->conditions = ParseUnaryExpr();
		whileStmt->block = ParseBlock(false);
		return whileStmt;
	}

	ASTElse* Parser::ParseElse(){
		if (trace) Trace("Parsing", "Else Statement");
		Expect(ELSE);
		int r = row;
		int c = col;
		Next();
		ASTElse* elseStmt = isolate->InsertToHeap(new ASTElse, ASTELSE);
		elseStmt->row = r;
		elseStmt->col = c;
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
		int r = row;
		int c = col;
		Next();
		ASTExpr* expr = ParseUnaryExpr();
		expr->row = r;
		expr->col = c;
		ASTIf* ifStmt = isolate->InsertToHeap(new ASTIf, ASTIF);
		AddRowCol(ifStmt);
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

	ASTExpr* Parser::ParseObjectInit(){
		Expect(NEW);
		Next();
		if (trace) Trace("Parsing", "New Object");
		Expect(IDENT);

		ASTObjectInit* call = isolate->InsertToHeap(new ASTObjectInit, OBJECT_INIT);
		AddRowCol(call);
		call->name = tok->raw;
		call->pos = pos;
		Next();
		Expect(LPAREN);
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

	ASTNode* Parser::ParseObject(){
		Expect(OBJECT);
		int r = row;
		int c = col;
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
		ASTObject* obj = isolate->InsertToHeap(new ASTObject, ASTOBJECT);
		obj->row = r;
		obj->col = c;
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
				obj->members.push_back(node);
			}
			else{
				VISIBILITY v = GetVisibility();
				node = NULL;
				if (tok->value == FUNC) node = ParseFunc();
				else node = ParseVar();
				node->visibility = v;
				obj->members.push_back(node);
			}
		}
		Expect(RBRACE);
		Next();
		return obj;
	}

	ASTExpr* Parser::ParseArray(ASTExpr* expr){
		if (tok->value == LBRACK && expr->type == IDENT){
			if (trace) Trace("Parsing", "Array");
			int r = row;
			int c = col;
			Next();
			ASTArrayMemberExpr* aryMem = isolate->InsertToHeap(new ASTArrayMemberExpr, ASTARRAY_MEMBER_EXPR);
			aryMem->row = r;
			aryMem->col = c;
			aryMem->name = expr->name;
			aryMem->member = ParseExpr();
			aryMem->value = expr;
			Expect(RBRACK);
			Next();
			return aryMem;
		}
		return expr;
	}

	ASTExpr* Parser::ParseFuncCall(ASTExpr* expr){
		// This will always be a function call
		if (tok->value == LPAREN && expr->type == IDENT){
			if (trace) Trace("Parsing", "Function call");

			ASTFuncCallExpr* call = isolate->InsertToHeap(new ASTFuncCallExpr, ASTFUNC_CALL_EXPR);
			AddRowCol(call);
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
		ASTIdent* ident = isolate->InsertToHeap(new ASTIdent, ASTIDENT);
		AddRowCol(ident);
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
					//ASTExpr* second = ParseOperand();
					ASTExpr* second = ParsePrimaryExpr();
					if (second->type != IDENT && second->type != FUNC_CALL) throw Error::INVALID_OBJECT_MEMBER;
					ASTObjectMemberChainExpr* chain = isolate->InsertToHeap(new ASTObjectMemberChainExpr, ASTOBJECT_MEMBER_CHAIN_EXPR);
					AddRowCol(chain);
					chain->object = first;
					chain->member = second;
					chain->name = '%' + first->name; // signal to not include in map
					return chain;
				}
				return first;
			}
			case kNULL: case BOOLEAN: case INT: case FLOAT: case DOUBLE: case CHAR: case STRING: case TRUE_LITERAL: case FALSE_LITERAL: {
				ASTLiterary* lit = isolate->InsertToHeap(new ASTLiterary, ASTLITERARY);
				AddRowCol(lit);
				lit->kind = tok->value;
				if (tok->value == FALSE_LITERAL || tok->value == TRUE_LITERAL) {
					lit->kind = BOOLEAN;
					lit->value = FALSE_LITERAL ? "false" : "true";
				}
				else if (tok->value == kNULL){
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
			ASTBinaryExpr* binary = isolate->InsertToHeap(new ASTBinaryExpr, ASTBINARY_EXPR);
			AddRowCol(binary);
			binary->op = isolate->InsertToHeap(new Token(tok->value), K_TOKEN);
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
				if (nonOp && !internal) throw Error::UNEXPECTED_OPERATOR;
				else if (nonOp && internal){
					Expect(MOD);
					Next();
					Expect(IDENT);
					std::string name = tok->raw;
					Next();
					Expect(LPAREN);
					Next();
					ASTExpr* expr = ParseExpr();
					Expect(RPAREN);
					Next();
					Expect(SEMICOLON);
					return Internal::ParseCall(expr, name, topScope);
				}
				ASTUnaryExpr* unary = isolate->InsertToHeap(new ASTUnaryExpr, ASTUNARY_EXPR);
				AddRowCol(unary);
				unary->pos = pos;
				unary->op = isolate->InsertToHeap(new Token(tok->value), K_TOKEN);
				unary->value = ParseUnaryExpr();
				return unary;
			}
			case LPAREN: {
				Next();
				ASTExpr* expr = ParseExpr();
				Expect(RPAREN);
				Next();

				if (tok->value == IDENT){ // CAST
					ASTCastExpr* castExpr = isolate->InsertToHeap(new ASTCastExpr, ASTCAST_EXPR);
					AddRowCol(castExpr);
					castExpr->value = ParseExpr();
					castExpr->to = expr;
					return castExpr;
				}

				return expr;
			}
			case LBRACK: {
				ASTArrayMemberExpr* ary = isolate->InsertToHeap(new ASTArrayMemberExpr, ASTARRAY_MEMBER_EXPR);
				AddRowCol(ary);
				return ary;
			}
			case NEW: {
				return ParseObjectInit();
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
			ASTBinaryExpr* binary = isolate->InsertToHeap(new ASTBinaryExpr, ASTBINARY_EXPR);
			AddRowCol(binary);
			binary->op = isolate->InsertToHeap(new Token(tok->value), K_TOKEN);
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

				ASTUnaryExpr* unary = isolate->InsertToHeap(new ASTUnaryExpr, ASTUNARY_EXPR);
				AddRowCol(unary);
				unary->op = isolate->InsertToHeap(new Token(tok->value), K_TOKEN);
				unary->pos = pos;
				Next();
				nonOp = true;
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
		ASTVar* var = isolate->InsertToHeap(new ASTVar, ASTVAR);
		AddRowCol(var);
		var->name = "return";
		var->stmt = ParseSimpleStmt();
		Expect(SEMICOLON);
		Next();
		return var;
	}

	/**
	 * int i = 0;
	 * var i = 0;
	 * int {
	 *    a;
	 *    b;
	 *    c = 10;
	 * }
	 */
	ASTNode* Parser::ParseVar(){
		ASTVar* var = isolate->InsertToHeap(new ASTVar, ASTVAR);
		AddRowCol(var);

		if (mode == STRICT){
			if (trace) Trace("Parsing", ("Var: " + tok->String()).c_str());
			var->varType = tok->value;
			Next();
		}
		else{
			if (trace) Trace("Parsing", ("Var: " + tok->raw).c_str());
			Next(); // eat var
		}
		if (tok->value == IDENT){	
			var->name = tok->raw;		
			var->stmt = ParseSimpleStmt();
		}
		else if (tok->value == LBRACE){
			ASTVarList* list = isolate->InsertToHeap(new ASTVarList, VARLIST);
			AddRowCol(list);
			TOKEN type = var->varType;
			Next(); // eat {
			while (tok->value != RBRACE){
				Expect(IDENT);
				var->name = tok->raw;
				var->stmt = ParseSimpleStmt();
				if (tok->value != RBRACE){
					Expect(SEMICOLON);
					Next();
				}
				list->vars.push_back(var);
				if (tok->value != RBRACE){
					var = isolate->InsertToHeap(new ASTVar, VAR);
					AddRowCol(var);
					var->varType = type;
				}
			}
			Next();
			Expect(SEMICOLON);
			Next();
			return list;
		}
		else{
			throw Error::PARSE_VAR_ERR;
		}
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
				Expect(SEMICOLON);
				Next();
				return aryMem;
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
			ASTVar* var = isolate->InsertToHeap(new ASTVar, ASTVAR);
			AddRowCol(var);
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
	 * 	Complete all the different statement types
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
				return NULL;
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
			if (stmt->type == VARLIST){
				ASTVarList* list = (ASTVarList*)stmt;
				for (int i = 0; i < list->vars.size(); i++){
					topScope->Insert(list->vars[i]);
				}
			}
			else{
				topScope->Insert(stmt);
			}
			if (tok->value == END || tok->value == RBRACE) break;
		}
	}

	/**
	 * {
	 *    ** List of statements **
	 * }
	 */
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

		ASTBlock* block = isolate->InsertToHeap(new ASTBlock, ASTBLOCK);
		AddRowCol(block);
		block->scope = topScope;

		if (printVariables) topScope->String();

		CloseScope();
		return block;
	}

	/**
	 * Lazy: func (param1, param2){}
	 * Strict: func ([type] param1, [type] param2){}
	 */
	void Parser::ParseFuncParams(ASTFunc* func){
		if (trace) Trace("Parsing", "Func Parameters");
		ASTParamVar* var = NULL;
		while (true){
			if (mode == STRICT){
				if (tok->value == RPAREN) return;
				if (!IsVarType()) throw Error::EXPECTED_VARIABLE_TYPE;
				TOKEN rType = tok->value;
				Next();
				Expect(IDENT);
				var = isolate->InsertToHeap(new ASTParamVar, ASTPARAM_VAR);
				AddRowCol(var);
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
					func->args.push_back(var);
					Next();
				}
				else if (tok->value == RPAREN) {
					func->args.push_back(var);
					break;
				}
				else Expect(RPAREN);
			}
			else{
				if (tok->value == RPAREN) return;
				Expect(IDENT);
				ASTVar* var = isolate->InsertToHeap(new ASTParamVar, ASTPARAM_VAR);
				AddRowCol(var);
				std::string name = tok->raw;
				Next();
				if (tok->value == LBRACK){
					Next();
					Expect(RBRACK);
					Next();
					var = isolate->InsertToHeap(new ASTArray(VAR), ASTARRAY);
					AddRowCol(var);
					var->name = name;
				}
				else{
					var->name = name;
				}

				if (tok->value == COMMA) {
					func->args.push_back(var);
					Next();
				}
				else if (tok->value == RPAREN) {
					func->args.push_back(var);
					break;
				}
				else Expect(RPAREN);
			}
		}
	}

	ASTFunc* Parser::ParseFunc(){
		Next(); // eat func
		Expect(IDENT);
		ASTFunc* func = isolate->InsertToHeap(new ASTFunc, ASTFUNC);
		AddRowCol(func);
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
	 * include "math";
	 *
	 * or
	 *
	 * import {
	 * 	"math";
	 * 	"os";
	 * }
	 * 
	 * include {
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
						ASTImport* import = isolate->InsertToHeap(new ASTImport, ASTIMPORT);
						AddRowCol(import);
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
						ASTInclude* include = isolate->InsertToHeap(new ASTInclude, ASTINCLUDE);
						AddRowCol(include);
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
					ASTImport* import = isolate->InsertToHeap(new ASTImport, ASTIMPORT);
					AddRowCol(import);
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
					ASTInclude* include = isolate->InsertToHeap(new ASTInclude, ASTINCLUDE);
					AddRowCol(include);
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
		Next();
		if (tok->value == HASH){
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
		else{ // if there isn't a #mode, default will be lazy
			mode = LAZY;
			reset = true;
		}
	}
} // namespace internal
}	// namespace Cobra