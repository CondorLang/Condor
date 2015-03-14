#ifndef TOKENS_H_
#define TOKENS_H_

#include <string>

namespace Cobra {
	namespace Tokens {

		#define RESERVED_WORDS " import include macro, var short bool char int float double long return if else null true false for do while each new this strict try catch finally throw export delete "

		#define TOK_IMPORT "import"
		#define TOK_INCLDUE "include"
		#define TOK_MACRO "macro"
		#define TOK_VAR "var"
		#define TOK_SHORT "short"
		#define TOK_BOOL "bool"
		#define TOK_CHAR "char"
		#define TOK_INT "int"
		#define TOK_FLOAT "float"
		#define TOK_DOUBLE "double"
		#define TOK_LONG "long"
		#define TOK_RETURN "return"
		#define TOK_IF "if"
		#define TOK_ELSE "else"
		#define TOK_NULL "null"
		#define TOK_TRUE "true"
		#define TOK_FALSE "false"
		#define TOK_FOR "for"
		#define TOK_DO "do"
		#define TOK_WHILE "while"
		#define TOK_EACH "each"
		#define TOK_NEW "new"
		#define TOK_THIS "this"
		#define TOK_STRICT "strict"
		#define TOK_TRY "try"
		#define TOK_CATCH "catch"
		#define TOK_FINALLY "finally"
		#define TOK_THROW "throw"
		#define TOK_EXPORT "export"
		#define TOK_DELETE "delete"
		
		typedef struct {
			int* pos;
			int* col;
			int* row;
			const char* raw;
		} Token;

		bool isReservedWord(const char* str);
		//bool isReservedWord(char* str);
	}
}

#endif // TOKENS_H_