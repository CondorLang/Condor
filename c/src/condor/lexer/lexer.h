// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The end user will not interact with this library.
 * This file is used to tokenize the code string
 *
 * User:
 * 	Syntax Analysis Only
 * 
 * Usage:
 * 	char* code = "var a = 10;";
 * 	Lexer* lexer = InitLexer(code);
 * 	Token token = GetNextToken(lexer); 	
 */

#ifndef LEXER_H_
#define LEXER_H_

#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "condor/mem/allocate.h"
#include "condor/token/token.h"

typedef struct LexerTracker{
	int tokenStart; // substr
	int tokenEnd; // substr
	int currentTokenPosition; // If -1, is EOF
} LexerTracker;

typedef struct Lexer{
	LexerTracker tracker;
	char* rawSourceCode;
	int length;
	char* currentTokenString;
} Lexer;

/**
 * Public Functions
 */
void InitLexer(Lexer* lexer, char* rawSourceCode);
void DestroyLexer(Lexer* lexer);
Token GetNextToken(Lexer* lexer);
char* GetCurrentTokenString(Lexer* lexer);
int CountTotalASTTokens(Lexer* lexer);
int CountTotalScopes(Lexer* lexer);
void ResetLexer(Lexer* lexer);

/**
 * Private Functions
 */
void SetNextTokenRange(Lexer* lexer);
char GetCurrentCharacter(Lexer* lexer);
char GetNextCharacter(Lexer* lexer);
char PeekNextCharacter(Lexer* lexer);
bool IsCharacter(char character, char charactersToCheck[]);
char* GetTokenString(Lexer* lexer);
bool CrawlIdentifierCharacters(Lexer* lexer);
bool CrawlSpaces(Lexer* lexer);
bool CrawlOperators(Lexer* lexer);
bool CrawlNumbers(Lexer* lexer);
bool CrawlString(Lexer* lexer);

void SetTokenStart(Lexer* lexer);
void SetTokenEnd(Lexer* lexer);

#endif // LEXER_H_
