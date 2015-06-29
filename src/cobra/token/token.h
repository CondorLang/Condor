#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

namespace Cobra {
	enum TOKEN{
		// Internal
		ILLEGAL = 255,
		END,
		EXPR,
		LITERARY,
		UNARY,
		BINARY,
		FUNC_CALL,
		FILE,
		BLOCK,
		ARRAY_MEMBER,
		OBJECT_MEMBER_CHAIN,
		CLASS,

		// Literals
		LITERAL_BEGIN,
		IDENT, // identifier
		INT, // 1
		FLOAT, // 1.0000000
		DOUBLE, // 1.0000000000000000
		BOOLEAN, // 1 or 0
		CHAR, // 'a'
		STRING, // "apple"
		ARRAY, // []
		LITERAL_END,

		// Operators
		OPERATOR_BEGIN,
		ADD, // +
		SUB, // -
		MUL, // *
		DIV, // /
		MOD, // %,
		HASH, // #

		AND, // &
		OR, // |
		XOR, // ^
		SHL, // <<
		SHR, // >>
		AND_NOT, // &^

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

		LAND, // &&
		LOR, // ||
		LARROW, // <-
		RARROW, // ->
		INC, // ++
		DEC, // --

		EQL, // ==
		LSS, // <
		GTR, // >
		ASSIGN, // =
		NOT, // !

		NEQ,      // !=
		LEQ,      // <=
		GEQ,      // >=
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

		STRUCT,
		SWITCH,
		TYPE,
		VAR,
		KEYWORD_END
	};	

	class Token
	{
	public:
		Token(TOKEN val){value = val;}
		~Token(){}
		TOKEN value;
		std::string raw;

		std::string String();
		int Precedence();
		bool IsKeyword(){return value > KEYWORD_BEGIN && value < KEYWORD_END;}
		bool IsLiteral(){return value > LITERAL_BEGIN && value < LITERAL_END;}
		bool IsOperator(){return value > OPERATOR_BEGIN && value < OPERATOR_END;}
		static Token* GetToken(std::string str);
	};
}

#endif // TOKEN_H_
