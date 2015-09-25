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
			case EXPR: return "kExpr";
			case LITERARY: return "kLiterary";
			case UNARY: return "kUnary";
			case BINARY: return "kBinary";
			case FUNC_CALL: return "kFuncCall";
			case OBJECT_INIT: return "kObjectInit";
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
			case ASTCAST_EXPR: return "kAstCastExpr";
			case SCANNER: return "kScanner";
			case SCOPE: return "kScope";
			case K_FUNC: return "kFunc";
			case HANDLE: return "kHandle";
			case ASTARRAY: return "kAstArray";
			case ASTLITERARY: return "kAstLiterary";
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
			case TRUE_LITERAL: return "true";
			case FALSE_LITERAL: return "false";
			case TRY: return "try";
			case THROW: return "throw";

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
	Token* Token::GetToken(Isolate* iso, std::string str){
		if (str.empty()) return Token::New(iso, ILLEGAL);
		else if (str == "var") return Token::New(iso, VAR);
		else if (str == "null") return Token::New(iso, kNULL);
		else if (str == "type") return Token::New(iso, TYPE);
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
		else return Token::New(iso, IDENT);
	}

	const char* Token::ToString(TOKEN token){
		Token* tok = new Token(token);
		const char* str = tok->String().c_str();
		delete tok;
		return str;
	}

	Token* Token::New(Isolate* iso, TOKEN val){
		Token* t = (Token*) iso->GetMemory(sizeof(Token));
		t->value = val;
		return t;
	}
} // namespace internal
} // namespace Cobra