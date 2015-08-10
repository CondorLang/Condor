#include "token.h"

namespace Cobra {
namespace internal{
	
	/**
	 * @brief Converts a Token to a string
	 * @return std::string
	 */
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
			case SCRIPT: return "kScript";
			case ASTFILE: return "kAstFile";
			case ASTFOR: return "kAstFor";
			case ASTWHILE: return "kAstWhile";
			case ASTELSE: return "kAstElse";
			case ASTIF: return "kAstIf";
			case ASTOBJECT: return "kAstObject";
			case ASTARRAY_MEMBER_EXPR: return "kAstArrayMemberExpr";
			case ASTFUNC_CALL_EXPR: return "kAstFuncCallExpr";
			case ASTIDENT: return "kAstIdent";
			case ASTOBJECT_MEMBER_CHAIN_EXPR: return "kAstObjectMemberChainExpr";
			case ASTBINARY_EXPR: return "kAstBinaryExpr";
			case ASTUNARY_EXPR: return "kAstUnaryExpr";
			case ASTVAR: return "kAstVar";
			case ASTBLOCK: return "kAstBlock";
			case ASTFUNC: return "kAstFunc";
			case ASTIMPORT: return "kAstImport";
			case ASTINCLUDE: return "kAstInclude";
			case SCANNER: return "kScanner";
			case SCOPE: return "kScope";
			case K_FUNC: return "kFunc";
			case HANDLE: return "kHandle";
			case ASTARRAY: return "kAstArray";
			case ASTLITERARY: return "kAstLiterary";
			case K_TOKEN: return "kToken";
			case PARSER: return "kParser";
			case CHECK: return "kCheck";

			case INT: return "int";
			case kNULL: return "null";
			case FLOAT: return "float";
			case DOUBLE: return "double";
			case BOOLEAN: return "boolean";
			case CHAR: return "char";
			case STRING: return "string";
			case CONSTRUCTOR: return "@";
			case PUBLIC: return "public";
			case PRIVATE: return "private";
			case PROTECTED: return "protected";
			case STATIC: return "static";
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
			case WHILE: return "while";
			case AS: return "as";
			case EXPORT: return "export";

			default: return "";
		}
	}

	/**
	 * @brief Get the tokens precedence
	 * @details This is used in the math portions.
	 * @return int 0 - 5 (0 being the lowest)
	 */
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

	/**
	 * @brief Get token by string
	 * @details Returns the Token* based on the inputed string
	 * 
	 * @param str std::string
	 * @return Token
	 */
	Token* Token::GetToken(std::string str){
		if (str.empty()) return new Token(ILLEGAL);
		else if (str == "var") return new Token(VAR);
		else if (str == "null") return new Token(kNULL);
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
		else if (str == "while") return new Token(WHILE);
		else if (str == "public") return new Token(PUBLIC);
		else if (str == "private") return new Token(PRIVATE);
		else if (str == "protected") return new Token(PROTECTED);
		else if (str == "static") return new Token(STATIC);
		else if (str == "as") return new Token(AS);
		else if (str == "export") return new Token(EXPORT);
		else return new Token(IDENT);
	}

	const char* Token::ToString(TOKEN token){
		Token* tok = new Token(token);
		const char* str = tok->String().c_str();
		delete tok;
		return str;
	}
} // namespace internal
} // namespace Cobra