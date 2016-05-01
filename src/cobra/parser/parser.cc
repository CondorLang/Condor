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
		allowNative = ALLOW_NATIVE;
		rootScope = Scope::New(isolate);
		scope = rootScope;
		printVariables = PRINT_VARIABLES;
		trace = TRACE_PARSER;
	}

	void Parser::SetInteral(bool isInternal){
		this->isInternal = isInternal;
		if (isInternal){
			allowNative = true;
			printVariables = false;
			trace = false;
		}
	}

	void Parser::SetDefaults(std::string* source){
		source = source;
		scanner = Scanner::New(isolate, source);
	}

	void Parser::SetRowCol(ASTNode* node){
		node->row = row;
		node->col = col;
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

	Scope* Parser::Parse(Isolate* iso, Scope* sc, Semantics* s){
		if (sc == NULL) throw Error::INTERNAL_SCOPE_ERROR;
		if (sc->IsParsed()) return sc;
		Parser* p = Parser::New(iso, &sc->raw);
		try {
			p->Parse();
		}
		catch (Error::ERROR e){
			s->row = p->row;
			s->col = p->col;
			throw e;
		}
		Scope* result = p->GetBaseScope();
		result->name = sc->name;
		result->owner = sc->owner;
		result->raw = sc->raw;
		result->outer = sc->outer;
		iso->FreeMemory(p, sizeof(Parser));
		iso->FreeMemory(sc, sizeof(Scope));
		result->SetParsed();
		return result;
	}

	void Parser::Trace(const char* name, const char* value){
		if (trace) printf("%s - %s\n", name, value);
	}

	void Parser::PrintTok(){
		printf("Debug: Raw: %s, Type: %s\n", tok->raw.c_str(), Token::ToString(tok->value).c_str());
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
      if (!isInList && tok != NULL) isInList = tok->value == va_arg(ap, int);
    }
    va_end(ap);
    return isInList;
	}

	bool Parser::IsOperator(){
		return Is(8, ADD, SUB, MOD, DIV, MUL, SHL, SHR, POW);
	}

	bool Parser::IsAssignment(){
		return Is(12, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN
								, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN
								, OR_ASSIGN, XOR_ASSIGN, SHL_ASSIGN
								, SHR_ASSIGN, AND_NOT_ASSIGN);
	}

	bool Parser::IsVarType(){
		return Is(11, INT, BOOLEAN, FLOAT, DOUBLE, LONG, CHAR, STRING, IDENT, TRUE_LITERAL, FALSE_LITERAL, kNULL);
	}

	bool Parser::IsBoolean(){
		return Is(11, LAND, LOR, EQL, LSS, GTR, NEQ, LEQ, GEQ, NOT, XOR, AND);
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
	void Parser::ParseImportOrInclude(bool eatTok){
		if (eatTok) Next(); // first token
		bool isImport = false;
		bool group = false;
		bool includeBrace = true;
		if (!Is(2, IMPORT, INCLUDE)) return;
		Trace("Parsing", "Imports");
		isImport = Is(1, IMPORT);
		while (true){
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
				SetRowCol(import);
				import->name = tok->raw;
				import->alias = ParseAlias();
				imports.push_back(import);
			}
			else {
				ASTInclude* include = ASTInclude::New(isolate);
				SetRowCol(include);
				include->name = tok->raw;
				include->alias = ParseAlias();
				includes.push_back(include);
			}
			if (Is(1, COMMA)){
				includeBrace = false;
				group = true;
			}
			if (!includeBrace && group && !Is(1, COMMA)) break;
			if (group && !Is(2, COMMA, RBRACE)) throw Error::INVALID_INCLUDE_IMPORT;
		}
		if (Is(1, RBRACE)) Next(); // eat missed }
		ParseImportOrInclude(false); // second wave
	}

	/**
	 * Allowed syntax:
	 * 
	 * 		as "string"
	 */
	std::string Parser::ParseAlias(){
		Next();
		if (Is(1, AS)){
			//throw Error::NOT_IMPLEMENTED;
			Next();
			Expect(IDENT);
			std::string result = tok->raw;
			Next();
			return result;
		}
		return "";
	}

	void Parser::ParseShallowStmtList(TOKEN terminator, int total){
		ASTNode* node = NULL;
		int i = 0;
		while (tok->value != terminator && tok->value != END){
			if (i == total) break;
			i++;
			bool isExport = false;
			std::vector<TOKEN> visibility;
			node = NULL; // reset node
			if (Is(1, EXPORT)) {
				isExport = true;
				Next();
			}
			if (Is(4, PUBLIC, STATIC, PRIVATE, PROTECTED)){
				if (isExport) throw Error::INVALID_USE_OF_EXPORT;
				while (Is(4, PUBLIC, STATIC, PRIVATE, PROTECTED)) {
					visibility.push_back(tok->value);
					Next();
				}
			}
			int type = tok->Int();
			switch (type){
				case LBRACE: { // for straggling braces
					Next();
					break;
				}
				case FUNC: {
					node = ParseFunc(); 
					break;
				}
				case INTERNAL: {
					node = ParseInternal();
					break;
				}
				case IDENT: {
					node = ParseIdentStart();
					break;
				}
				case INT: case BOOLEAN: 
				case FLOAT: case DOUBLE: 
				case CHAR: case STRING: 
				case LONG: 
				case TRUE_LITERAL: 
				case FALSE_LITERAL: 
				case kNULL: case VAR: {
					if (tok->IsRawNumber()){ // for inline stmts
						node = ParseExpr();
						node->print = true;
						break;	
					}
					std::vector<ASTVar*> list = ParseVarList();
					for (int i = 0; i < list.size(); i++){
						list[i]->isExport = isExport;
						list[i]->visibility.insert(list[i]->visibility.end(), visibility.begin(), visibility.end());
						scope->Insert(list[i]);
					}
					break;
				}
				case FOR: {
					node = ParseForExpr();
					break;
				}
				case WHILE: {
					node = ParseWhile();
					break;
				}
				case TRY: {
					node = ParseTryCatch();
					break;
				}
				case THROW: {
					node = ParseThrow();
					break;
				}
				case IF: {
					node = ParseIf();
					break;
				}
				case DELETE: {
					node = ParseDelete();
					break;
				}
				case SWITCH: {
					node = ParseSwitch();
					break;
				}
				case OBJECT: {
					node = ParseObject();
					break;
				}
				case RETURN: {
					node = ParseReturn();
					break;
				}
				case BREAK: {
					node = ParseBreak();
					break;
				}
				default: {
					TOKEN t = tok->value;
					node = ParseExpr();
					if (node != NULL) node->print = true;
					if (tok->value == t) throw Error::RESERVED_KEYWORD;
				}
			}
			if (node != NULL){
				node->isExport = isExport;
				node->visibility.insert(node->visibility.end(), visibility.begin(), visibility.end());
				scope->Insert(node);
			}
		}
	}

	ASTNode* Parser::ParseBreak(){
		Trace("Parsing", "Break");
		Expect(BREAK);
		ASTBreak* b = ASTBreak::New(isolate);
		Next();
		Expect(SEMICOLON);
		Next();
		return b;
	}

	/** 
	 * Allowed syntax:
	 *
	 * func name([type] ident, name){
	 * 		...
	 * }
	 */
	ASTFunc* Parser::ParseFunc(){
		Trace("Parsing", "Func");
		Expect(FUNC);
		Next();
		Expect(IDENT);
		ASTFunc* func = ASTFunc::New(isolate);
		SetRowCol(func);
		func->name = tok->raw;
		Next();
		std::vector<ASTVar*> vars = ParseFuncArgs();
		if (vars.size() > 0) func->args.insert(func->args.end(), vars.begin(), vars.end());
		func->scope = LazyParseBody();
		func->scope->owner = func;
		func->scope->name = func->name;
		return func;
	}

	Scope* Parser::LazyParseBody(){
		Trace("Storing", "Body for later");
		bool oneStmt = !Is(1, LBRACE);
		if (oneStmt){
			Scope* scope = Scope::New(isolate);
			scope->SetParsed();
			int start = pos;
			OpenScope(scope);
			ParseShallowStmtList(SEMICOLON, 1);
			scope->raw = scanner->Substr(start, pos);
			CloseScope();
			return scope;	
		}
		Expect(LBRACE);
		Next();
		int start = pos - tok->Length() + 1;
		if (Is(1, INTERNAL)) start--;
		int braceDepth = 1;
		while (true){
			if (Is(1, LBRACE)) braceDepth++;
			if (Is(1, RBRACE)) braceDepth--;
			if (braceDepth == 0) break;
			if (Is(1, END)) throw Error::UNEXPECTED_END_OF_FILE;
			Next(); // eat all tokens
		}
		int end = pos;
		Scope* scope = Scope::New(isolate);
		scope->raw = scanner->Substr(start, end);
		Next(); // eat }
		return scope;
	}

	ASTNode* Parser::ParseIdentStart(){
		Trace("Parsing", "Ident Start");
		ASTExpr* expr = ParseExpr(false);
		if (IsAssignment()) {
			ASTBinaryExpr* binary = ASTBinaryExpr::New(isolate);
			SetRowCol(binary);
			binary->left = expr;
			binary->cast = NULL;
			binary->op = tok->value;
			if (!tok->IsAssign()) binary->print = true;
			Next();
			binary->right = ParseExpr();
			if (Is(1, SEMICOLON)) Next();
			return binary;
		}
		if (Is(1, IDENT)){ 
			Trace("Parsing Var", tok->raw.c_str());
			ASTVar* var = ASTVar::New(isolate);
			ASTLiteral* lit = (ASTLiteral*) expr;
			var->baseName = lit->value;
			var->baseType = OBJECT;
			var->name = tok->raw;
			Next();
			if (IsAssignment() && !Is(1, ASSIGN)) throw Error::INVALID_ASSIGNMENT;
			else if (IsAssignment()) Next(); // eat =
			else if (Is(1, PERIOD)){
				ASTBinaryExpr* binary = ASTBinaryExpr::New(isolate);
				SetRowCol(binary);
				lit->value = var->name;
				binary->left = lit;
				var->Free(isolate);
				binary->op = PERIOD;
				Next();
				binary->right = ParseExpr();
				binary->cast = NULL;
				Expect(SEMICOLON); // but don't eat
				return binary;
			}
			lit->Free(isolate);
			var->value = ParseExpr();
			if (var->value != NULL && var->value->type == FUNC_CALL && ((ASTFuncCall*)var->value)->isInit) var->isObject = true;
			if (Is(1, SEMICOLON)) Next();
			return var;
		}
		if (Is(1, SEMICOLON)) Next();
		return expr;
	}

	/**
	 * Allowed syntax:
	 *
	 * var a = 10;
	 * int a = 10;
	 * var {
	 * 		a = 10;
	 * 		b = 20;
	 * }
	 */
	std::vector<ASTVar*> Parser::ParseVarList(){
		std::vector<ASTVar*> vars;
		TOKEN base = tok->value;
		Next();
		bool isBrace = Is(1, LBRACE);
		if (Is(1, LBRACE)) Next();
		bool breakOut = false;
		bool isArray = Is(1, LBRACK);
		if (isArray){
			Next();
			Expect(RBRACK);
			Next();
		}
		while (true){
			if (Is(1, RBRACE)) {
				breakOut = true;
				break;
			}
			Expect(IDENT);
			ASTVar* var = ASTVar::New(isolate);
			SetRowCol(var);
			var->baseType = base;
			var->name = tok->raw;
			var->isArray = isArray;
			Next();
			Trace("Parsing Var", var->name.c_str());
			var->op = tok->value;
			if (!IsAssignment() && !Is(1, SEMICOLON)) throw Error::INVALID_OPERATOR;
			if (!Is(1, SEMICOLON)){
				Next();
				var->value = ParseExpr();
			}
			if (var->value == NULL) var->value = ASTUndefined::New(isolate);
			var->isObject = var->value->type == FUNC_CALL && ((ASTFuncCall*)var->value)->isInit; 
			vars.push_back(var);
			if (!isBrace) return vars;
		}
		if (breakOut) Next();
		return vars;
	}

	ASTExpr* Parser::ParseExpr(bool semi){
		Trace("Parsing", "Expression");
		ASTExpr* expr = ParseBinaryExpr(semi);
		if (semi && Is(1, SEMICOLON)) Next(); // loose semicolon
		return expr;
	}

	ASTExpr* Parser::ParseBinaryExpr(bool semi){
		Trace("Parsing", "Binary Expression");
		ASTExpr* expr = NULL;
		bool pre = true;
		TOKEN unary = UNDEFINED;
		bool incdec = Is(2, INC, DEC);
		ASTLiteral* cast = NULL;
		if (incdec) {
			unary = tok->value;
			Next();
		}
		if (Is(1, LPAREN)){ // type casting
			Next();
			ASTExpr* paren = ParseExpr();
			
			if (paren->type == LITERAL && ((ASTLiteral*) paren)->isCast){
				cast = (ASTLiteral*) paren;
				if (cast->type != LITERAL) throw Error::UNKNOWN_CAST_TYPE;
				SetRowCol(cast);
				Expect(RPAREN);
				Next();
				Trace("Parsing", "Type cast");
			}
			else{
				expr = paren;
				expr->isInParen = true;
				SetRowCol(expr);
				Expect(RPAREN);
				Next();
			}			
		}
		if (IsVarType()) expr = ParseVarType();
		if (Is(1, LPAREN)) expr = ParseFuncCall(expr);
		if (Is(2, INC, DEC) || incdec){
			ASTLiteral* lit = (ASTLiteral*) expr;
			if (!incdec) {
				lit->unary = tok->value;
				Next();
			}
			else {
				lit->unary = unary;
				lit->isPost = false;
			}
		}
		if (Is(1, LBRACK)){
			Next(); // eat [
			Trace("Parsing", "Array");
			if (expr != NULL){
				ASTLiteral* var = ASTLiteral::New(isolate);
				var->value = ((ASTLiteral*) expr)->value;
				var->member = ParseExpr();
				var->litType = IDENT;
				Expect(RBRACK);
				Next();
				return var;
			}

			ASTArray* ary = ASTArray::New(isolate);
			while (!Is(1, RBRACK)){
				ary->members.push_back(ParseExpr());
				if (Is(1, COMMA)) Next();
			}
			Next();
			expr->cast = cast;
			return ary;
		}
		if (IsOperator() || IsBoolean() || Is(1, PERIOD)) { // needs to be last to catch all lingering operators
			ASTBinaryExpr* binary = ASTBinaryExpr::New(isolate);
			SetRowCol(binary);
			binary->left = expr;
			expr->cast = cast;
			binary->op = tok->value;
			binary->isChain = Is(1, PERIOD);
			Trace("Parsing Operator", Token::ToString(tok->value).c_str());
			Next();
			binary->right = ParseExpr(semi);
			return binary;
		}
		if (Is(1, NEW)){
			Next();
			ASTExpr* expr = ParseVarType();
			ASTFuncCall* call = (ASTFuncCall*) ParseFuncCall(expr);
			call->isInit = true;
			call->cast = cast;
			return call;
		}
		if (Is(1, INTERNAL)){
			return (ASTExpr*) ParseInternal();
		}
		if (expr != NULL){
			expr->cast = cast;
		}
		return expr;
	}

	ASTExpr* Parser::ParseVarType(){
		ASTLiteral* lit = ASTLiteral::New(isolate);
		SetRowCol(lit);
		lit->value = tok->raw;
		if (tok->raw.length() > 0) Trace("Parsing Literal", tok->raw.c_str());
		else Trace("Parsing Literal", tok->String().c_str());
		lit->litType = tok->value;
		if (lit->value.size() == 0) lit->value = tok->String();
		if (lit->value == Token::ToString(lit->litType)) lit->isCast = true;
		Next();
		return lit;
	}

	ASTExpr* Parser::ParseFuncCall(ASTExpr* expr){
		if (expr == NULL) throw Error::INVALID_FUNCTION_CALL;
		Next();
		ASTFuncCall* call = ASTFuncCall::New(isolate);
		SetRowCol(call);
		ASTLiteral* lit = (ASTLiteral*) expr;
		call->name = lit->value;
		isolate->FreeMemory(expr, sizeof(ASTExpr));
		while (true){
			ASTExpr* e = ParseExpr();
			if (e != NULL) call->params.push_back(e);
			if (Is(1, RPAREN)) break;
			if (!Is(1, COMMA)) throw Error::EXPECTED_PARAMETER;
			Next();
		}
		Next(); // eat )
		return call;
	}

	ASTExpr* Parser::ParseForExpr(){
		ASTForExpr* expr = ASTForExpr::New(isolate);
		SetRowCol(expr);
		Next();
		Expect(LPAREN);
		Next();
		expr->var = ParseVarList()[0];
		expr->condition = ParseBoolean();
		expr->tick = ParseExpr();
		Expect(RPAREN);
		Next();
		expr->scope = LazyParseBody();
		expr->scope->owner = expr;
		return expr;
	}

	ASTExpr* Parser::ParseBoolean(){
		Trace("Parsing", "Boolean");
		ASTBinaryExpr* binary = (ASTBinaryExpr*) ParseBinaryExpr();
		SetRowCol(binary);
		binary->isBoolean = true;
		return binary;
	}

	ASTExpr* Parser::ParseWhile(){
		Trace("Parsing", "While");
		ASTWhileExpr* expr = ASTWhileExpr::New(isolate);
		SetRowCol(expr);
		Next();
		Expect(LPAREN);
		Next();
		expr->condition = ParseBoolean();
		Expect(RPAREN);
		Next();
		expr->scope = LazyParseBody();
		expr->scope->owner = expr;
		return expr;
	}

	ASTExpr* Parser::ParseTryCatch(){
		Trace("Parsing", "Try Catch");
		ASTTryCatchExpr* expr = ASTTryCatchExpr::New(isolate);
		SetRowCol(expr);
		Next();
		expr->tryScope = LazyParseBody();
		expr->tryScope->owner = expr;
		Expect(CATCH);
		Next();
		std::vector<ASTVar*> vars = ParseFuncArgs();
		if (vars.size() > 0) expr->catchParams.insert(expr->catchParams.end(), vars.begin(), vars.end());
		expr->catchScope = LazyParseBody();
		expr->catchScope->owner = expr;
		return expr;
	}

	std::vector<ASTVar*> Parser::ParseFuncArgs(){
		Expect(LPAREN);
		Next();
		std::vector<ASTVar*> vars;
		while (true){
			if (Is(1, RPAREN)) break;
			if (!Is(3, IDENT, RPAREN, VAR) && !IsVarType()) throw Error::INVALID_ARGUMENT_TYPE;
			if (Is(2, IDENT, VAR) || IsVarType()){
				std::string first = tok->raw;
				TOKEN firstType = tok->value;
				if (first.length() == 0) first = tok->String();
				Next();
				std::string second = "";
				if (Is(1, IDENT)) {
					second = tok->raw;
					Next();
				}
				ASTVar* var = ASTVar::New(isolate);
				SetRowCol(var);
				var->name = second;
				var->baseName = first;
				var->baseType = firstType;
				vars.push_back(var);
			}
			if (Is(1, RPAREN)) break;
			Expect(COMMA);
			Next();
		}
		Next();
		return vars;
	}

	ASTExpr* Parser::ParseThrow(){
		Trace("Parsing", "Throw");
		Next();
		ASTThrow* stmt = ASTThrow::New(isolate);
		SetRowCol(stmt);
		stmt->expr = ParseExpr();
		return stmt;
	}

	ASTExpr* Parser::ParseIf(){
		Trace("Parsing", "If");
		ASTIf* stmt = ASTIf::New(isolate);
		SetRowCol(stmt);
		if (Is(1, ELSE)){
			Next();
			ASTLiteral* lit = ASTLiteral::New(isolate);
			SetRowCol(lit);
			lit->value = "true";
			lit->litType = TRUE_LITERAL;
			stmt->condition = lit;
			stmt->scope = LazyParseBody();
			stmt->isElse = true;
			return stmt;
		}
		if (Is(1, IF)){
			Next();
			Expect(LPAREN);
			Next();
			stmt->condition = ParseBoolean();
			Expect(RPAREN);
			Next();
			stmt->scope = LazyParseBody();
			stmt->scope->owner = stmt;
			while (Is(2, ELSE, IF)){
				bool el = Is(1, ELSE);
				ASTIf* ifs = (ASTIf*) ParseIf();
				stmt->elseIfs.push_back(ifs);
				if (el) return stmt;
			}
			return stmt;
		}
		isolate->FreeMemory(stmt, sizeof(ASTIf));
		return NULL;
	}

	ASTNode* Parser::ParseDelete(){
		Trace("Parsing", "Delete");
		Next();
		Expect(IDENT);
		ASTDelete* del = ASTDelete::New(isolate);
		SetRowCol(del);
		del->node = ParseVarType();
		Expect(SEMICOLON);
		Next();
		return del;
	}

	ASTExpr* Parser::ParseSwitch(){
		Trace("Parsing", "Switch");
		Next();
		ASTSwitch* expr = ASTSwitch::New(isolate);
		SetRowCol(expr);
		Expect(LPAREN);
		Next();
		expr->value = ParseExpr();
		Expect(RPAREN);
		Next();
		Expect(LBRACE);
		Next();
		while (true){
			ASTCase* e = (ASTCase*) ParseCase();
			if (e == NULL) break;
			expr->cases.push_back(e);
		}
		Expect(RBRACE);
		Next();
		return expr;
	}

	ASTExpr* Parser::ParseCase(){
		ASTCase* expr = ASTCase::New(isolate);
		SetRowCol(expr);
		if (Is(1, CASE)){
			Trace("Parsing", "Case");
			Next();
			expr->condition = (ASTLiteral*) ParseVarType();
		}
		else if (Is(1, DEFAULT)){
			Trace("Parsing", "Default");
			Next();
			expr->isDefault = true;
		}
		else {
			isolate->FreeMemory(expr, sizeof(ASTCase));
			return NULL;
		}
		Expect(COLON);
		Next();
		expr->scope = LazyParseBody();
		expr->scope->owner = expr;
		return expr;
	}

	ASTNode* Parser::ParseObject(){
		Next();
		bool extend = Is(1, DOLLAR);
		if (extend) Next();
		Expect(IDENT);
		Trace("Parsing Object", tok->raw.c_str());
		ASTObject* obj = ASTObject::New(isolate);
		obj->name = tok->raw;
		obj->extend = extend;
		SetRowCol(obj);
		Next();
		obj->scope = LazyParseBody();
		obj->scope->owner = obj;
		obj->scope->name = obj->name;
		obj->scope->outer = scope;
		return obj;
	}

	void Parser::OpenScope(Scope* sc){
		sc->outer = scope;
		scope = sc;
	}

	void Parser::CloseScope(){
		if (scope == NULL) throw Error::INTERNAL_SCOPE_ERROR;
		scope = scope->outer;
	}

	// TODO: Disallow for allow-native on a non internal file
	ASTNode* Parser::ParseInternal(){
		Trace("Parsing Internal Func Call", tok->raw.c_str());
		ASTLiteral* expr = ASTLiteral::New(isolate);
		SetRowCol(expr);
		expr->value = tok->raw;
		ASTFuncCall* call = (ASTFuncCall* ) ParseFuncCall(expr);
		call->isInternal = true;
		if (call->name[0] == '%') call->name.erase(call->name.begin());
		if (Is(1, SEMICOLON)) Next();
		else throw Error::UNEXPECTED_CHARACTER;
		return call;
	}

	ASTNode* Parser::ParseReturn(){
		Trace("Parsing", "Return");
		ASTVar* var = ASTVar::New(isolate);
		SetRowCol(var);
		var->name = tok->String();
		Next();
		var->value = ParseExpr();
		if (var->value != NULL && var->value->type == FUNC_CALL && ((ASTFuncCall*)var->value)->isInit) var->isObject = true;
		if (Is(1, SEMICOLON)) Next();
		return var;
	}
} // namespace internal
}	// namespace Cobra