// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The end user will not interact with this library
 *
 * User:
 * 	All phases
 * 
 * Usage:
 * 	Token is an enum
 * 
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#include <string.h>
#include <stdbool.h>

#define CREATE_TOKEN_LIST(T) \
	T(IDENTIFIER, "@IDENTIFIER") \
	T(NUMBER, "@NUMBER") \
	T(BEGIN_OPERATOR, "#BEGIN_OPERATOR") \
	T(SEMICOLON, ";") \
	T(COLON, ";") \
	T(CONSTRUCTOR, "@") \
	T(BEGIN_BINARY_OPERATOR, "@BEGIN_BINARY_OPERATOR") \
	T(ADD, "+") \
	T(SUB, "-") \
	T(MUL, "*") \
	T(DIV, "/") \
	T(MOD, "%") \
	T(POW, "^") \
	T(END_BINARY_OPERATOR, "@END_BINARY_OPERATOR") \
	T(HASH, "#") \
	T(DOLLAR, "$") \
	T(TICK, "`") \
	T(AND, "&") \
	T(OR, "|") \
	T(XOR, "^") \
	T(SHL, "<<") \
	T(SHR, ">>") \
	T(AND_NOT, "&^") \
	T(BEGIN_ASSIGNMENT, "#BEGIN_ASSIGNMENT") \
	T(ASSIGN, "=") \
	T(ADD_ASSIGN, "+=") \
	T(SUB_ASSIGN, "-=") \
	T(MUL_ASSIGN, "*=") \
	T(DIV_ASSIGN, "/=") \
	T(MOD_ASSIGN, "%=") \
	T(AND_ASSIGN, "&=") \
	T(OR_ASSIGN, "|=") \
	T(XOR_ASSIGN, "^=") \
	T(SHL_ASSIGN, "<<=") \
	T(SHR_ASSIGN, ">>=") \
	T(AND_NOT_ASSIGN, "&^=") \
	T(END_ASSIGNMENT, "#END_ASSIGNMENT") \
	T(BEGIN_BOOLEAN_COMPARISON, "#BEGIN_BOOLEAN_COMPARISON") \
	T(LAND, "&&") \
	T(LOR, "||") \
	T(EQL, "==") \
	T(NEQ, "!=") \
	T(LEQ, "<=") \
	T(GEQ, ">=") \
	T(NOT, "!") \
	T(LESS, "<") \
	T(GREATER, ">") \
	T(END_BOOLEAN_COMPARISON, "#END_BOOLEAN_COMPARISON") \
	T(LARROW, "<-") \
	T(RARROW, "->") \
	T(INC, "++") \
	T(DEC, "--") \
	T(ELLIPSIS, "...") \
	T(LPAREN, "(") \
	T(RPAREN, ")") \
	T(LBRACK, "[") \
	T(RBRACK, "]") \
	T(LBRACE, "{") \
	T(RBRACE, "}") \
	T(COMMA, ",") \
	T(PERIOD, ".") \
	T(END_OPERATOR, "#END_OPERATOR") \
	T(NULL_LITERAL, "null") \
	T(BEGIN_NUMBER, "#BEGIN_NUMBER") \
	T(BOOLEAN, "bool") \
	T(BYTE, "byte") \
	T(SHORT, "short") \
	T(INT, "int") \
	T(FLOAT, "float") \
	T(DOUBLE, "double") \
	T(LONG, "long") \
	T(END_NUMBER, "#END_NUMBER") \
	T(BEGIN_STRING, "#BEGIN_STRING") \
	T(CHAR, "char") \
	T(STRING, "string") \
	T(END_STRING, "#END_STRING") \
	T(KEYWORD_BEGIN, "#KEYWORD_BEGIN") \
	T(PUBLIC, "public") \
	T(PRIVATE, "private") \
	T(STATIC, "static") \
	T(PROTECTED, "protected") \
	T(BREAK, "break") \
	T(CASE, "case") \
	T(CONST, "const") \
	T(CONTINUE, "continue") \
	T(DEFAULT, "default") \
	T(ELSE, "else") \
	T(FOR, "for") \
	T(FROM, "from") \
	T(FUNC, "func") \
	T(IF, "if") \
	T(IMPORT, "import") \
	T(INCLUDE, "include") \
	T(MAP, "map") \
	T(NEW, "new") \
	T(OBJECT, "object") \
	T(RETURN, "return") \
	T(TRUE_LITERAL, "true") \
	T(FALSE_LITERAL, "false") \
	T(STRUCT, "struct") \
	T(SWITCH, "switch") \
	T(THROW, "throw") \
	T(TRY, "try") \
	T(CATCH, "catch") \
	T(VAR, "var") \
	T(WHILE, "while") \
	T(AS, "as") \
	T(EXPORT, "export") \
	T(DELETE, "delete") \
	T(INTERNAL, "internal") \
	T(DO, "do") \
	T(KEYWORD_END, "#KEYWORD_END") \
	T(UNDEFINED, "undefined") \
	T(BINARY, "@BINARY")

#define T_ENUM(x, name) x, // {enum}
#define T_ARRAY(x, name) {x, name}, // {enum, string value}

enum Tokens {CREATE_TOKEN_LIST(T_ENUM) TOTAL_TOKENS};

typedef enum Tokens Token;

/**
 * Convert a char* to a TOKEN.
 */
Token StringToToken(char* value);
char* TokenToString(Token tok);
bool IsAssignment(Token tok);
bool IsBinaryOperator(Token tok);
bool IsBooleanOperator(Token tok);
bool IsNumber(Token tok);
bool IsString(Token tok);

#endif // TOKEN_H_