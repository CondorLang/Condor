#ifndef SIMPLE_H_
#define SIMPLE_H_

#include <string>

int goodSimpleSize = 12;

std::string goodSimple[] = {
	"#mode \"strict\"; func isTrue(){return true;}", // 1 func, 1 bool
	"#mode \"lazy\"; func add(a, b){return a + b;}", // lazy, 1 func
	"#mode \"strict\"; func add(int a, int b){return a + b;}", // strict, 1 func
	"#mode \"lazy\"; func add(a, b){return a + b;}func minus(a, b){return a - b;}", // lazy, 2 func
	"#mode \"strict\"; func add(int a,int b){return a+b;}func minus(int a,int b){return a-b;}", // 2 func, minified
	"#mode \"lazy\"; func add(a, b){a - b;}", // no return
	"#mode \"lazy\"; func add(a,b){a+b;}func minus(a,b){a-b;}", // 2 func, minified, no return
	"#mode \"lazy\"; func add(a,b){return a+b;}func main(){return add(1, 2);}", // 2 func, 1 no args, call func
	"#mode \"lazy\"; func add(a,b){return a+b;}func main(){return add(add(1,22), 2);}", // 2 func, 1 no args, call nested func
	"#mode \"lazy\"; func add(a,b){return a + (1 + b) + 2;}", // 2 func, 1 prec paren stmt
	"#mode \"strict\"; int global = 10; func add(int a,int b){return a+b;}", // 1 global variable
	"#mode \"strict\"; func first(int a1[]){a1[0] = 10; return a1[0];}" // 1 func, 1 array, access member in ary
};

#endif // SIMPLE_H_
