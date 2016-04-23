#include "token.h"
#include "cobra/mem/isolate.h"

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
			case TOK: return "token";
			case EXPR: return "kExpr";
			case LITERARY: return "kLiterary";
			case UNARY: return "kUnary";
			case BINARY: return "kBinary";
			case FUNC_CALL: return "kFuncCall";
			case OBJECT_INSTANCE: return "kObjectInit";
			case FILE: return "kFile";
			case BLOCK: return "kBlock";
			case IDENT: return "ident";
			case ARRAY_MEMBER: return "karrayMember";
			case ARRAY: return "array";
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
			case ASTCAST_EXPR: return "kAstCastExpr";
			case ASTNULL: return "kAstNull";
			case AST_NOT: return "kAstNot";
			case SCANNER: return "kScanner";
			case SCOPE: return "kScope";
			case K_FUNC: return "kFunc";
			case HANDLE: return "kHandle";
			case ASTARRAY: return "kAstArray";
			case LITERAL: return "kLiteral";
			case K_TOKEN: return "kToken";
			case PARSER: return "kParser";
			case CHECK: return "kCheck";
			case VARLIST: return "kVarList";
			case UNDEFINED: return "undefined";
			case API_STRING: return "kApiString";
			case API_SCRIPT: return "kApiScript";
			case INTERNAL_TOKEN: return "kInternalToken";
			case ASTPARAM_VAR: return "kASTParamVar";
			case TRY_CATCH: return "kTryCatch";
			case POINTER: return "kPointer";

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
			case POW: return "^";
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
			case DOLLAR: return "$";
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
			case VAR: return "var";
			case OBJECT: return "object";
			case WHILE: return "while";
			case AS: return "as";
			case EXPORT: return "export";
			case TRUE_LITERAL: return "true";
			case FALSE_LITERAL: return "false";
			case TRY: return "try";
			case THROW: return "throw";
			case DELETE: return "delete";
			case CATCH: return "catch";
			case INTERNAL: return "internal";

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
			case LPAREN: case RPAREN: return 0;
			case ASSIGN: case ADD_ASSIGN: 
				case SUB_ASSIGN: case MUL_ASSIGN: 
				case DIV_ASSIGN: case MOD_ASSIGN: 
				case AND_ASSIGN: case OR_ASSIGN: 
				case XOR_ASSIGN: case SHL_ASSIGN: 
				case SHR_ASSIGN: case AND_NOT_ASSIGN: 
					return 1;
			case LOR: return 2;
			case LAND: return 3;
			case EQL: case NEQ: case LSS: case LEQ: case GTR: case GEQ: return 4;
			case ADD: case SUB: case OR: case XOR: return 5;
			case MUL: case DIV: case MOD: case SHL: case SHR: case AND: case AND_NOT: return 6;
			case POW: return 7;
			case PERIOD: return 8;
			default: return -1;
		}
	}

	/**
	 * @brief Get token by string
	 * @details Returns the Token* based on the inputed string
	 * 
	 * @param str std::string
	 * @return Token
	 */
	Token* Token::GetToken(Isolate* iso, std::string str){
		if (str.empty()) return Token::New(iso, ILLEGAL);
		else if (str == "var") return Token::New(iso, VAR);
		else if (str == "null") return Token::New(iso, kNULL);
		else if (str == "switch") return Token::New(iso, SWITCH);
		else if (str == "struct") return Token::New(iso, STRUCT);
		else if (str == "return") return Token::New(iso, RETURN);
		else if (str == "map") return Token::New(iso, MAP);
		else if (str == "include") return Token::New(iso, INCLUDE);
		else if (str == "import") return Token::New(iso, IMPORT);
		else if (str == "if") return Token::New(iso, IF);
		else if (str == "goto") return Token::New(iso, GOTO);
		else if (str == "go") return Token::New(iso, GO);
		else if (str == "func") return Token::New(iso, FUNC);
		else if (str == "for") return Token::New(iso, FOR);
		else if (str == "else") return Token::New(iso, ELSE);
		else if (str == "default") return Token::New(iso, DEFAULT);
		else if (str == "continue") return Token::New(iso, CONTINUE);
		else if (str == "const") return Token::New(iso, CONST);
		else if (str == "case") return Token::New(iso, CASE);
		else if (str == "break") return Token::New(iso, BREAK);
		else if (str == "int") return Token::New(iso, INT);
		else if (str == "bool") return Token::New(iso, BOOLEAN);
		else if (str == "float") return Token::New(iso, FLOAT);
		else if (str == "double") return Token::New(iso, DOUBLE);
		else if (str == "char") return Token::New(iso, CHAR);
		else if (str == "string") return Token::New(iso, STRING);
		else if (str == "object") return Token::New(iso, OBJECT);
		else if (str == "new") return Token::New(iso, NEW);
		else if (str == "while") return Token::New(iso, WHILE);
		else if (str == "public") return Token::New(iso, PUBLIC);
		else if (str == "private") return Token::New(iso, PRIVATE);
		else if (str == "protected") return Token::New(iso, PROTECTED);
		else if (str == "static") return Token::New(iso, STATIC);
		else if (str == "as") return Token::New(iso, AS);
		else if (str == "export") return Token::New(iso, EXPORT);
		else if (str == "true") return Token::New(iso, TRUE_LITERAL);
		else if (str == "false") return Token::New(iso, FALSE_LITERAL);
		else if (str == "try") return Token::New(iso, TRY);
		else if (str == "throw") return Token::New(iso, THROW);
		else if (str == "delete") return Token::New(iso, DELETE);
		else if (str == "catch") return Token::New(iso, CATCH);
		else if (str == "undefined") return Token::New(iso, UNDEFINED);
		else return Token::New(iso, IDENT);
	}

	std::string Token::ToString(TOKEN token){
		Token tok(token);
		return tok.String();
	}

	Token* Token::New(Isolate* iso, TOKEN val){
		void* p = iso->GetMemory(sizeof(Token));
		Token* t = new(p) Token(val);
		return t;
	}

	int Token::Length(){
		if (raw.length() > 0) return (int) raw.length();
		return String().length();
	}
} // namespace internal
} // namespace Cobra