#include "parser.h"

using namespace Cobra;

Parser::Parser(std::string rawCode){
	lexer = new Lexer(rawCode);
	row = 0;
	col = 0;
	found = "";
}

Parser::~Parser(){
	delete lexer;
	for (std::map<std::string, ASTNode*>::iterator it = program.begin(); it != program.end(); ++it){
	  delete it->second;
	}
	while (!stack.empty()){
		delete stack.top();
		stack.pop();
	}
}

void Parser::nextToken(){
	lexer->nextToken(&token);
	row = lexer->row;
	col = lexer->col - 1; // start of the next token
	lexeme = token.lexeme.c_str();
}

void Parser::Parse(){

	int type = (int)token.type; // enums can't be in a switch

	switch (type){
		case Tokens::END: return;
		case Tokens::IDENTIFIER: {
			// add to callStack
			ASTNode* expr = ParseExpression();
			break;
		}
		case Tokens::KEYWORD: {
			if (token.lexeme == "func"){
				ParseFunction();
			}
			else if (token.lexeme == "object"){
				ParseObject();
			}
			break;
		}
		case Tokens::CONSTANT: {
			break;
		}
		case Tokens::STRING: {
			break;
		}
		case Tokens::DELIMITER: {
			break;
		}
		case Tokens::OPERATOR: {
			break;
		}
		case Tokens::PUNCTUATOR: {
			break;
		}
		default: {
			if (token.lexeme.empty()){
				break;
			}
			throw Errors::UNDEFINED;			
		}
	}

	nextToken();

	Parse();
}

/**
 * Accepted Values:
 * 		func [identifier] ([arg], [arg]){
 * 			** block **
 * 		}
 *
 * or
 *
 * 		func [identifier] ([type] [arg], [type] [arg]){
 * 			** block **
 * 		}
 */
void Parser::ParseFunction(){
	nextToken();
	if (token.type != Tokens::IDENTIFIER) throw Errors::EXPECTED_FUNCTION_NAME;
	std::string funcName = token.lexeme;
	nextToken();
	if (token.lexeme != "(") throw Errors::EXPECTED_LEFT_PARENTHESE;
	nextToken(); // eat the left paran;

	ASTFunction* function = new ASTFunction;
	function->name = funcName;
	scope = funcName;

	int argCount = 0;

	while (true){ // crawl for arguments 
		if (token.lexeme == ")") {
			nextToken();
			break;
		}
		if (token.type != Tokens::IDENTIFIER){
			throw Errors::UNEXPECTED_CHARACTER;
		}
		ASTFunctionArg* arg = new ASTFunctionArg;
		arg->name = token.lexeme;
		function->args[argCount++] = arg;

		nextToken();
		if (token.lexeme == ","){
			nextToken();
			continue;
		}
		else if (token.lexeme == ")"){
			nextToken(); // eat )
			break;
		}
		else{
			throw Errors::UNEXPECTED_CHARACTER;
		}
	}

	program[funcName] = function;
	function->block = ParseBlock();
}

ASTBlock* Parser::ParseBlock(){

	if (token.lexeme != "{") throw Errors::EXPECTED_LEFT_BRACE;
	nextToken(); // eat {

	ASTBlock* block = new ASTBlock;

	while (token.lexeme != "}"){
		if (token.type == Tokens::IDENTIFIER){
			std::string name = token.lexeme;
			nextToken();
			if (token.lexeme == "("){

			}
			else if (token.lexeme == "=") {

			}
			else{

			}
		}
		else if (token.type == Tokens::KEYWORD){
			std::string varName = "";
			if (token.lexeme == "return"){
				varName = token.lexeme;

				nextToken(); // eat return

				ASTNode* expr = ParseExpression();
				block->map[varName] = expr;
				block->vector.push_back(ASTFactory::CreateVar(varName, expr));
			}
			/**
			 * var [identifier] = expr
			 */
			else if (token.lexeme == "var"){

				nextToken();
				if (token.type != Tokens::IDENTIFIER) throw Errors::UNEXPECTED_CHARACTER;
				varName = token.lexeme;

				nextToken();
				if (token.lexeme != "=") throw Errors::EXPECTED_EQUALS;
				nextToken(); // eat =

				ASTNode* expr = ParseExpression();
				block->map[varName] = expr;
				block->vector.push_back(ASTFactory::CreateVar(varName, expr));
			}
		}
		nextToken();
	}
	return block;
}

void Parser::ParseObject(){
	nextToken(); // eat keyword
	if (token.type != Tokens::IDENTIFIER) throw Errors::EXPRECTED_OBJECT_NAME;

	ASTObject* obj = new ASTObject;
	obj->name = token.lexeme;
	//printf("Parsing object: %s\n", obj->name.c_str());

	nextToken();
	if (token.lexeme != "{") throw Errors::EXPECTED_LEFT_BRACE;

	/**
	 * Parse body
	 */
	while (token.lexeme != "}"){
		nextToken();
	}
	program[obj->name] = obj;
}

ASTNode* Parser::ParseExpression(){

	while (token.lexeme != ";"){
		if (token.type == Tokens::END) throw Errors::MISSING_SEMI_COLON;

		stack.push(ParseValue(&token));
		nextToken();
	}
	ASTNode* top = NULL;
	ASTNode* hold = NULL;
	ASTNode* current = NULL;

	for (int i = stack.size(); i > 0; i--){
		top = stack.top();

		if (top->type == "var"){
			ASTVar* var = (ASTVar*) top;
			ASTFunction* func = (ASTFunction*)program[scope];
			ASTBlock* funcBlock = (ASTBlock*)func->block;
			std::map<std::string, ASTNode*> block = funcBlock->map;
			std::map<std::string, ASTNode*>::iterator it;
			it = block.find(var->name.c_str());

			if (it != block.end()){
				//top = (ASTString*)it->second;
			}
			else{
				int count = 0;
				bool foundValue = false;
				while (true){
					ASTFunctionArg* arg = func->args[count++];
					if (arg == NULL) break;
					else {
						if (arg->name == var->name){
							foundValue = true;
							break;
						}
					}
				}
				if (!foundValue) {
					found = var->name;
					throw Errors::UNDEFINED;
				}
			}	
		}

		stack.pop();
		if (i % 2 == 0){
			if (top->type != "operator") throw Errors::EXPECTED_OPERATOR;
			if (current == NULL) throw Errors::EXPECTED_OPERATOR;
			if (hold == NULL) {
				top->Left = current;
				current = NULL;
				hold = top;
			}
		}
		else{
			if (hold != NULL){
				hold->Right = top;
				current = hold;
				hold = NULL;
			}
			if (current == NULL)
				current = top;
		}
	}

	return current;
}

ASTNode* Parser::ParseValue(Tokens::Token* tok){
	ASTNode* value = NULL;

	if (tok->type == Tokens::STRING){
		ASTString* str = new ASTString;
		str->value = tok->lexeme;
		value = str;
	}
	else if (tok->type == Tokens::IDENTIFIER){
		ASTVar* var = new ASTVar;
		var->name = tok->lexeme;
		var->value = GetScopeVarByName(var->name);
		value = var;
	}
	else if (tok->type == Tokens::CONSTANT){
		if (tok->lexeme.size() == 1 && (tok->lexeme == "0" || tok->lexeme == "1")){
			ASTBoolean* boolean = new ASTBoolean;
			bool val = true;
			if (tok->lexeme == "0") val = false;
			boolean->value = val;
			value = boolean;
		}
		else if (tok->lexeme.find('.')){
			double val = atoi(tok->lexeme.c_str());
			ASTDouble* doubleVal = new ASTDouble;
			doubleVal->value = val;
			value = doubleVal;
		}
		else{
			int val = atoi(tok->lexeme.c_str());
			ASTInt* intVal = new ASTInt;
			intVal->value = val;
			value = intVal;
		}
	}
	else if (tok->type == Tokens::KEYWORD) throw Errors::UNDEFINED;
	else if (tok->type == Tokens::OPERATOR){
		char operatorType = tok->lexeme[0];
		ASTOperator* op = new ASTOperator;
		switch (operatorType){
			case '+': op->value = PLUS; break;
			case '-': op->value = MINUS; break;
			case '/': op->value = DIVIDE; break;
			case '*': op->value = MULTIPLY; break;
			case '^': op->value = POWER; break;
			case '=': op->value = EQUALS; break;
			case '!': op->value = EXMARK; break;
			case '>': {
				if (tok->lexeme.size() >= 2 && tok->lexeme[1] == '='){
					op->value = GREATER_THAN_EQ; break;
				}
				else{
					op->value = GREATER_THAN; break;
				}
			}
			case '<': {
				if (tok->lexeme.size() >= 2 && tok->lexeme[1] == '='){
					op->value = LESS_THAN_EQ; break;
				}
				else{
					op->value = LESS_THAN; break;
				}
			}
			default: throw Errors::UNDEFINED;
		}
		value = op;
	}
	else if (tok->type == Tokens::DELIMITER){
		// todo
	}

	return value;
}

ASTNode* Parser::GetScopeVarByName(std::string name){
	ASTFunction* func = (ASTFunction*)program[scope];
	ASTBlock* funcBlock = (ASTBlock*) func->block;
	std::map<std::string, ASTNode*> block = funcBlock->map;
	std::map<std::string, ASTNode*>::iterator it;
	it = block.find(name);
	if (it != block.end()){
		return it->second;
	}
	else{
		int count = 0;
		while (true){
			ASTFunctionArg* arg = func->args[count++];
			if (arg == NULL) break;
			else {
				if (arg->name == name){
					return arg;
				}
			}
		}
		return NULL;
	}
}

ASTNode* Parser::GetVarByName(std::string func, std::string name){
	scope = func.c_str();
	return GetScopeVarByName(name);
}

ASTFunction* Parser::GetFunctionByName(std::string name){
	return (ASTFunction*)program[name];
}

bool Parser::HasFunction(std::string name){
	return program.find(name.c_str()) != program.end();
}

bool Parser::Expect(std::string value){
	return token.lexeme == value;
}

void Compile(){
	std::string fileStr = "";
	std::ifstream in("test/test.cb");
	std::string line;
	while(getline(in, line)) {
	  fileStr += line;
	}

	Parser* parser = new Parser(fileStr.c_str());
	try {
		parser->Parse();
		// Executer* exe = new Executer(parser);
		// exe->Run();
	}
	catch (Errors::ERROR_MSG e){
		const char* errMsg = Errors::toString(e);
		if (parser->found.empty()){
			printf("%d:%d - %s, found %s\n", parser->row, parser->col, errMsg, parser->lexeme);
		}
		else{
			printf("%d:%d - %s, found %s\n", parser->row, parser->col, errMsg, parser->found.c_str());
		}
	}
	delete parser;
}