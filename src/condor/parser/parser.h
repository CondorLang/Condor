// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <stdarg.h>

#include "condor/assert.h"
#include "condor/mem/isolate.h"
#include "condor/token/token.h"
#include "condor/ast/node.h"
#include "condor/ast/scope.h"
#include "condor/scanner/scanner.h"
#include "condor/types/strings/string.h"
#include "condor/semantics/semantics.h"

namespace Condor {
namespace internal{

	class Semantics;

	class Parser
	{
	private:
		Isolate* isolate;
		std::string* source;
		Scanner* scanner;
		Token* tok;
		Scope* rootScope;
		Scope* scope;
		int pos;
		int row;
		int col;
		bool reset;
		bool nonOp;
		bool isInline;
		bool isInternal;
		bool printVariables;
		bool trace;
		bool allowNative;

		void SetDefaults(std::string* source);
		void SetRowCol(ASTNode* node);
		void Next();
		void Expect(TOKEN val);
		bool Is(int argc, ...);
		void PrintTok();
		void Trace(const char* name, const char* value);
		void ParseImportOrInclude(bool eatTok = true);
		std::string ParseAlias();
		void ParseShallowStmtList(TOKEN terminator = END, int total = -1);
		bool IsOperator();
		bool IsBoolean();
		bool IsAssignment();
		bool IsVarType();
		ASTFunc* ParseFunc();
		Scope* LazyParseBody();
		ASTNode* ParseIdentStart();
		std::vector<ASTVar*> ParseVarList();
		ASTExpr* ParseExpr(bool semi = true);
		ASTExpr* ParseBinaryExpr(bool semi = true);
		ASTExpr* ParseVarType();
		ASTExpr* ParseFuncCall(ASTExpr* expr);
		ASTExpr* ParseForExpr();
		ASTExpr* ParseBoolean();
		ASTExpr* ParseWhile();
		ASTExpr* ParseTryCatch();
		std::vector<ASTVar*> ParseFuncArgs();
		ASTExpr* ParseThrow();
		ASTExpr* ParseIf();
		ASTNode* ParseDelete();
		ASTExpr* ParseSwitch();
		ASTExpr* ParseCase();
		ASTNode* ParseObject();
		void OpenScope(Scope* sc);
		void CloseScope();
		ASTExpr* ParseInternal();
		ASTNode* ParseReturn();
		ASTNode* ParseBreak();

	public:
		Parser(Isolate* iso);
		~Parser();
		static Parser* New(Isolate* isolate, std::string* source);
		std::vector<ASTImport*> imports;
		std::vector<ASTInclude*> includes;
		
		void SetInteral(bool isInternal);
		bool IsInternal(){return isInternal;}
		void SetInline(bool isInline){isInline = isInline;}
		void Parse();
		static Scope* Parse(Isolate* iso, Scope* sc, Semantics* s);
		Scope* GetBaseScope(){return rootScope;}
		std::string* GetSource(){return source == NULL ? scanner->src : source;}

		int Pos;
		int Row;
		int Col;
		Token* expected;
		
	};
} // namespace internal
}

#endif // PARSER_H_
