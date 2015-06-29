#include "token.h"

namespace Cobra {
	
	std::string Token::String(){
		int t = (int)value;
		switch (t){
			// internal
			case ILLEGAL: return "illegal";
			case EXPR: return "kExpr";
			case LITERARY: return "kLiterary";
			case UNARY: return "kUnary";
			case BINARY: return "kBinary";
			case FUNC_CALL: return "kFuncCall";
			case FILE: return "kFile";
			case BLOCK: return "kBlock";
			case IDENT: return "ident";
			case ARRAY_MEMBER: return "karrayMember";
			case OBJECT_MEMBER_CHAIN: return "kObjectMemberChain";
			case CLASS: return "kClass";

			case INT: return "int";
			case FLOAT: return "float";
			case DOUBLE: return "double";
			case BOOLEAN: return "boolean";
			case CHAR: return "char";
			case STRING: return "string";
			case ADD: return "+";
			case SUB: return "-";
			case MUL: return "*";
			case DIV: return "/";
			case MOD: return "%";
			case HASH: return "#";
			case AND: return "&";
			case OR: return "|";
			case XOR: return "^";
			case SHL: return "<<";
			case SHR: return ">>";
			case AND_NOT: return "&^";
			case ADD_ASSIGN: return "+=";
			case SUB_ASSIGN: return "-=";
			case DIV_ASSIGN: return "/=";
			case MUL_ASSIGN: return "*=";
			case MOD_ASSIGN: return "%/";
			case AND_ASSIGN: return "&=";
			case OR_ASSIGN: return "|=";
			case XOR_ASSIGN: return "^=";
			case SHL_ASSIGN: return "<<=";
			case SHR_ASSIGN: return ">>=";
			case AND_NOT_ASSIGN: return "&^=";
			case LAND: return "&&";
			case LOR: return "||";
			case LARROW: return "<-";
			case RARROW: return ">-";
			case INC: return "++";
			case DEC: return "--";
			case EQL: return "==";
			case LSS: return "<";
			case GTR: return ">";
			case ASSIGN: return "=";
			case NOT: return "!";
			case NEQ: return "!=";
			case LEQ: return "<=";
			case GEQ: return ">=";
			case ELLIPSIS: return "...";
			case LPAREN: return "(";
			case LBRACK: return "[";
			case LBRACE: return "{";
			case COMMA: return ",";
			case PERIOD: return ".";
			case RPAREN: return ")";
			case RBRACK: return "]";
			case RBRACE: return "}";
			case SEMICOLON: return ";";
			case COLON: return ":";
			case BREAK: return "break";
			case CASE: return "case";
			case CONST: return "const";
			case CONTINUE: return "continue";
			case DEFAULT: return "default";
			case ELSE: return "else";
			case FOR: return "for";
			case FUNC: return "func";
			case GO: return "go";
			case GOTO: return "goto";
			case IF: return "if";
			case IMPORT: return "import";
			case INCLUDE: return "include";
			case MAP: return "map";
			case NEW: return "new";
			case RETURN: return "return";
			case STRUCT: return "struct";
			case SWITCH: return "switch";
			case TYPE: return "type";
			case VAR: return "var";
			case OBJECT: return "object";

			default: return "";
		}
	}

	int Token::Precedence(){
		int t = (int) value;
		switch (t){
			case LOR: return 1;
			case LAND: return 2;
			case EQL: case NEQ: case LSS: case LEQ: case GTR: case GEQ: return 3;
			case ADD: case SUB: case OR: case XOR: return 4;
			case MUL: case DIV: case MOD: case SHL: case SHR: case AND: case AND_NOT: return 5;
			default: return 0;
		}
		return 0;
	}

	Token* Token::GetToken(std::string str){
		if (str.empty()) return new Token(ILLEGAL);
		else if (str == "var") return new Token(VAR);
		else if (str == "type") return new Token(TYPE);
		else if (str == "switch") return new Token(SWITCH);
		else if (str == "struct") return new Token(STRUCT);
		else if (str == "return") return new Token(RETURN);
		else if (str == "map") return new Token(MAP);
		else if (str == "include") return new Token(INCLUDE);
		else if (str == "import") return new Token(IMPORT);
		else if (str == "if") return new Token(IF);
		else if (str == "goto") return new Token(GOTO);
		else if (str == "go") return new Token(GO);
		else if (str == "func") return new Token(FUNC);
		else if (str == "for") return new Token(FOR);
		else if (str == "else") return new Token(ELSE);
		else if (str == "default") return new Token(DEFAULT);
		else if (str == "continue") return new Token(CONTINUE);
		else if (str == "const") return new Token(CONST);
		else if (str == "case") return new Token(CASE);
		else if (str == "break") return new Token(BREAK);
		else if (str == "int") return new Token(INT);
		else if (str == "bool") return new Token(BOOLEAN);
		else if (str == "float") return new Token(FLOAT);
		else if (str == "double") return new Token(DOUBLE);
		else if (str == "char") return new Token(CHAR);
		else if (str == "string") return new Token(STRING);
		else if (str == "object") return new Token(OBJECT);
		else if (str == "new") return new Token(NEW);
		else return new Token(IDENT);
	}
} // namespace Cobra