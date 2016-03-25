#ifndef CODEGEN_H_
#define CODEGEN_H_

// Reference: http://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
// 		http://www.zynamics.com/binnavi.html
// 		http://www.delorie.com/djgpp/doc/ug/asm/calling.html
// 		http://www.tutorialspoint.com/assembly_programming/
// 		http://www.idryman.org/blog/2014/12/02/writing-64-bit-assembly-on-mac-os-x/
// 		http://www.tutorialspoint.com/assembly_programming/assembly_variables.htm

#include "cobra/ast/ast.h"
#include "cobra/token/token.h"
#include <string>

namespace Cobra {
namespace internal{

	class Codegen
	{
	private:
		void ScanScope();
		std::string code;
		int tabs;
		bool print;

		void Space(){code += " ";}
		void NewLine(){code += "\n";}
		void Colon(){code += ":";}
		void Comma(){code += ",";}
		void StartParam(){code += "esp";}
		void Param(int val);
		void LocalParameter(){code += "ebp";}
		void NoParams(){code += "sub esp, 0";}
		void Tab();
		void CNLT(){Colon(); NewLine(); Tab();}
		void Append(std::string txt){code += txt;}
		void Append(const char* txt){code += txt;}

		void GenerateFunc(ASTFunc* func);
		void ConstructFuncDecl();

	public:
		Codegen(){}
		~Codegen(){}
		static Codegen* New(Isolate* isolate);
		Isolate* isolate;
		ASTFile* kFile;
		void Process(ASTFile* file);
	};

} // namespace internal
} // namespace Cobra

#endif // CODEGEN_H_