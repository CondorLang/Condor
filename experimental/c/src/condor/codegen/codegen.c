#include "codegen.h"

int CodeGen_Add(int a, int b){
	int result;
	asm(
		"  movl %2,%0;"
		"  addl %1,%0;"
		: "=&r" (result)
		: "r" (b), "r" (a)
	);
	return result;
}

int CodeGen_Sub(int a, int b){
	int result;
	asm(
		"  movl %2,%0;"
		"  subl %1,%0;"
		: "=&r" (result)
		: "r" (b), "r" (a)
	);
	return result;
}

char* CodeGen_DeclareVariable(char* name, Token dataType){
	
	return "test";
}