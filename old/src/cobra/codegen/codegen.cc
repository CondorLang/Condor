#include "codegen.h"
#include "cobra/mem/isolate.h"
#include "symbols.h"
#include "cobra/flags.h"

namespace Cobra {
namespace internal{

	Codegen* Codegen::New(Isolate* isolate){
		void* p = isolate->GetMemory(sizeof(Codegen));
		Codegen* n = new(p) Codegen();
		n->isolate = isolate;
		n->tabs = 0;
		n->print = PRINT_ASM;
		n->print = true; // debug
		return n;
	}

	void Codegen::Process(ASTFile* file){
		kFile = file;
		ConstructFuncDecl();
		ScanScope();
		if (print) printf("ASM Code: \n\n%s\n", code.c_str());
	}

	void Codegen::Tab(){
		for (int i = 0; i < tabs; i++){
			code += "\t";
		}
	}

	void Codegen::ConstructFuncDecl(){
		Append(Symbol::sTEXT);
		NewLine();
		tabs++;

		for (int i = 0; i < kFile->scope->Size(); i++){
			ASTNode* node = kFile->scope->Get(i);
			if (node->type == FUNC){
				Tab();
				Append(Symbol::sGLOBAL);
				Space();
				Append(node->name);
				Append(" ; Declare Func " + node->name);
				NewLine();
			}
		}
		NewLine();
		tabs--;
	}

	void Codegen::ScanScope(){
		for (int i = 0; i < kFile->scope->Size(); i++){
			ASTNode* node = kFile->scope->Get(i);
			switch ((int) node->type){
				case FUNC: GenerateFunc((ASTFunc*) node); break;
			}
		}
	}

	void Codegen::Param(int val){
		code += "[";
		LocalParameter();
		Space();
		code += "+";
		Space();
		code += std::to_string(val);
		code += "]";
	}

	void Codegen::GenerateFunc(ASTFunc* func){
		NewLine();
		Append(func->name);
		Colon();
		tabs++;
		NewLine();
		if (func->args.size() > 0){
			Tab();
			Append(Symbol::sPUSH);
			Space();
			Append(Symbol::sARG);
			Space();
			Append("; Save EBP");
			NewLine();
			Tab();
			Append(Symbol::sMOVE);
			Space();
			LocalParameter();
			Comma();
			Space();
			StartParam();
			NewLine();
			Tab();
			NoParams();
			NewLine();
		}
		for (int i = 0; i < func->args.size(); i++){
			Tab();
			Append(Symbol::sMOVE);
			Space();
			Append(func->args[i]->name);
			Comma();
			Param((i + 1) * 8);
			NewLine();
		}
		NewLine();
	}

} // namespace internal
} // namespace Cobra