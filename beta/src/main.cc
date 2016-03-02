#include <stdio.h>

#include "cobra/parser/parser.h"

int main(int argc, const char** argv){
	std::string code = "func b(a,b){return a + b;}";
	Parser p(&code);
	p.Parse();
}