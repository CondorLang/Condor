#include <Condor.h>

int main(){
	// Scan("var a = 10;");
	Scan("var a = 10; var b = 100;");
	// Scan("var a = 10; var b = 100; var apple = 100.0;");
	// Scan("var test = 10 / \"b\"");
	// Scan("for (var i = 0; i < 100; i++) {var d = 100;}");
	// Scan("while (1 == 1 || 2 == 2 || 3 == 3 || 4 == 4) {for (var i = 0; i < 100; i++){int d = 200;}}");
}
