#include <Condor.h>

int main(){
	// Scan("var a = 10.0;");
	// Scan("var a = 10; var b = 100;");
	// Scan("var b = 10; var c = 100.0; var a = 10 + 10 + 10 + b + c;");
	// Scan("var a = 10; var b = 100; var apple = 100.0; var banana = 1 + 1 + a;");
	// Scan("var test = 10 / \"b\"");
	// Scan("var test = \"b\"");
	// Scan("var a = 10; for (var i = 0; i < 100; i++) {var d = 100;}");
	// Scan("while (1 == 1 || 2 == 2 || 3 == 3 || 4 == 4) {for (var i = 0; i < 100; i++){int d = 200;}}");
	// Scan("var i = 10; for (i = 0; i < 10; i++){i += 2;}");
	// Scan("var i = 0; for (i = 0; i < 10; i++){for (i = 0; i < 10; i++){i += 2;}}");
	Scan("var i = 0; switch (i) {case 0: return false;}");
}
