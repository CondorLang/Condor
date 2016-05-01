#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

namespace Cobra {
namespace internal{

	class Isolate;

	/**
	 * @brief The basic enum of all Tokens
	 */
	enum TOKEN{
		// Internal
		ILLEGAL = 255,
		TOK,
		END,
		EXPR,
		LITERARY,
		UNARY,
		BINARY,
		FUNC_CALL,
		OBJECT_INSTANCE,
		FILE,
		BLOCK,
		ARRAY_MEMBER,
		OBJECT_MEMBER_CHAIN,
		CLASS,
		SCRIPT,
		ASTFILE,
		ASTFOR,
		ASTWHILE,
		ASTELSE,
		ASTIF,
		ASTOBJECT,
		ASTARRAY_MEMBER_EXPR,
		ASTFUNC_CALL_EXPR,
		ASTIDENT,
		ASTOBJECT_MEMBER_CHAIN_EXPR,
		ASTBINARY_EXPR,
		ASTUNARY_EXPR,
		ASTVAR,
		ASTBLOCK,
		ASTFUNC,
		ASTIMPORT,
		ASTINCLUDE,
		ASTCAST_EXPR,
		ASTNULL,
		AST_NOT,
		SCANNER,
		SCOPE,
		K_FUNC,
		HANDLE,
		ASTARRAY,
		LITERAL,
		K_TOKEN,
		PARSER,
		CHECK,
		VARLIST,
		UNDEFINED,
		API_STRING,
		API_SCRIPT,
		INTERNAL_TOKEN,
		ASTPARAM_VAR,
		TRY_CATCH,
		POINTER,

		// Literals
		LITERAL_BEGIN,
		kNULL,
		IDENT, // identifier

		BEGIN_NUMBER,
		INT, // 1
		FLOAT, // 1.0000000
		DOUBLE, // 1.0000000000000000
		LONG, 
		BOOLEAN, // 1 or 0

		BEGIN_STRING,
		CHAR, // 'a'
		END_NUMBER,

		STRING, // "apple"
		END_STRING,


		ARRAY, // []
		LITERAL_END,

		CONSTRUCTOR, // @
		PUBLIC,
		STATIC,
		PROTECTED,
		PRIVATE,

		// Operators
		OPERATOR_BEGIN,
		ADD, // +
		SUB, // -
		MUL, // *
		DIV, // /
		MOD, // %,
		POW, // ^
		HASH, // #
		DOLLAR, // $
		TICK, // `

		AND, // &
		OR, // |
		XOR, // ^
		SHL, // <<
		SHR, // >>
		AND_NOT, // &^

		BEGIN_ASSIGNMENT,

		ASSIGN, // =

		ADD_ASSIGN, // +=
		SUB_ASSIGN, // -=
		MUL_ASSIGN, // *=
		DIV_ASSIGN, // /=
		MOD_ASSIGN, // %=

		AND_ASSIGN, // &=
		OR_ASSIGN, // |=
		XOR_ASSIGN, // ^=
		SHL_ASSIGN, // <<=
		SHR_ASSIGN, // >>=
		AND_NOT_ASSIGN, // &^=

		END_ASSIGNMENT,

		BOOLEAN_COMPARISON_START,

		LAND, // &&
		LOR, // ||
		EQL, // ==
		NEQ, // !=
		LEQ, // <=
		GEQ, // >=
		NOT, // !
		LSS, // <
		GTR, // >

		BOOLEAN_COMPARISON_END,

		LARROW, // <-
		RARROW, // ->
		INC, // ++
		DEC, // --
		ELLIPSIS, // ...

		LPAREN, // (
		LBRACK, // [
		LBRACE, // {
		COMMA,  // ,
		PERIOD, // .

		RPAREN,    // )
		RBRACK,    // ]
		RBRACE,    // }
		SEMICOLON, // ;
		COLON,     // :
		OPERATOR_END,

		// Keywords
		KEYWORD_BEGIN,
		BREAK,
		CASE,
		CONST,
		CONTINUE,

		DEFAULT,
		ELSE,
		FOR,

		FUNC,
		GO,
		GOTO,
		IF,
		IMPORT,
		INCLUDE,

		MAP,
		NEW,
		OBJECT,
		RETURN,
		TRUE_LITERAL,
		FALSE_LITERAL,

		STRUCT,
		SWITCH,
		THROW,
		TRY,
		CATCH,
		VAR,
		WHILE,
		AS,
		EXPORT,
		DELETE,
		INTERNAL,
		KEYWORD_END
	};	

	/**
	 * @brief The token wrapper
	 * @details The token wrapper is used in many classes. This
	 * is the base for all the tokens.
	 * 
	 * @param val TOKEN
	 */
	class Token
	{
	public:
		Token(TOKEN val){value = val;}
		Token(){}
		static Token* New(Isolate* iso, TOKEN val);
		~Token(){}
		TOKEN value;
		std::string raw;

		std::string String();
		int Int(){return (int) value;}
		int Length();
		int Precedence();
		bool IsKeyword(){return value > KEYWORD_BEGIN && value < KEYWORD_END;}
		bool IsLiteral(){return value > LITERAL_BEGIN && value < LITERAL_END;}
		bool IsOperator(){return value > OPERATOR_BEGIN && value < OPERATOR_END;}
		bool IsBinaryComparison(){return value > BOOLEAN_COMPARISON_START && value < BOOLEAN_COMPARISON_END;}
		bool IsNumber(){return value > BEGIN_NUMBER && value < END_NUMBER;}
		bool IsRawNumber();
		bool IsString(){return value > BEGIN_STRING && value < END_STRING;}
		bool IsAssign(){return value > BEGIN_ASSIGNMENT && value < END_ASSIGNMENT;}
		static Token* GetToken(Isolate* iso, std::string str);
		static std::string ToString(TOKEN token);
	};
} // namespace internal{
}

#endif // TOKEN_H_
