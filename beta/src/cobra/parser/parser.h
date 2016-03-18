#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <stdarg.h>

#include "cobra/mem/isolate.h"
#include "cobra/token/token.h"
#include "cobra/ast/node.h"
#include "cobra/scanner/scanner.h"
#include "cobra/types/strings/string.h"

namespace Cobra {
namespace internal{

	class Parser
	{
	private:
		Isolate* isolate;
		std::string* source;
		Scanner* scanner;
		Token* tok;
		int pos;
		int row;
		int col;
		bool reset;
		bool nonOp;
		bool isInline;
		bool isInternal;
		bool printVariables;
		bool trace;
		std::vector<ASTImport*> imports;
		std::vector<ASTInclude*> includes;

		void SetDefaults(std::string* source);
		void Next();
		void Expect(TOKEN val);
		bool Is(int argc, ...);
		void PrintTok();
		void Trace(const char* name, const char* value);
		void ParseImportOrInclude();
		std::string ParseAlias();
		void ParseShallowStmtList();
		void ParseFunc();

	public:
		Parser(Isolate* iso);
		~Parser();
		static Parser* New(Isolate* isolate, std::string* source);
		void SetInteral(bool isInternal){isInternal = isInternal;}
		void SetInline(bool isInline){isInline = isInline;}
		void Parse();

		int Pos;
		int Row;
		int Col;
		Token* expected;
		
	};
} // namespace internal
}

#endif // PARSER_H_
