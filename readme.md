<!-- ![StoneLang Title](https://github.com/chaseWillden/StoneLang/blob/master/StoneLang%20Title.png) -->
# StoneLang [![Build Status][travis-image]][travis-url]

StoneLang is intended to be another simple and intuitive programming language. The syntax is familiar by design, but allows for the combining of a few concepts.

Documentation
-------------
See [Wiki](https://github.com/chaseWillden/StoneLang/wiki) for documentation.

Contributing:
-------------
See [Contributing](https://github.com/chaseWillden/StoneLang/wiki/Contributing) page in the wiki
See [Developers](https://github.com/chaseWillden/StoneLang/wiki#developers) for building instructions.

[travis-url]: https://travis-ci.org/chaseWillden/StoneLang/
[travis-image]: https://img.shields.io/travis/chaseWillden/StoneLang/master.svg?style=flat

Examples
--------
### Hello World
```go
import "console"
println("Hello World");
```
### Object Oriented
```go
import "console"

object Fruit{
	private string _name;
	func Fruit(string name) this._name = name;
	func getName() return this._name;
	func toString() {
		return "The name of this fruit is: " + this._name;
	}
}

var apple = new Fruit("Apple");
Fruit banana = new Fruit("Banana");
println("First fruit: " + apple.getName() + ", Second fruit: " + banana.getName());
```
### Read File
```go
import "fs"
string contents = fs.readFile("readme.md");
```

Todo:
------
 - node.h 28 - Create a Expand() function which expands the node and the tree
 - execute.cc 115 - Figure out how to implement recursion
 - execute.cc 204 - Set row and col for tracking
 - execute.cc 471 - Implement bitwise
 - parser.cc 892 - Disallow for allow-native on a non internal file
 - scanner.cc 466 - Include Octal, Hex, UTF-8, UTF-16, see https://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
 - internal.cc 140 - Throw Error
 - internal.cc 152 - Throw Error
 - internal.cc 164 - Throw Error
 - internal.cc 272 - Move to a macro
 - semantics.cc 298 - Verify this is ok and always true
 - semantics.cc 672 - Is this true always?
 - semantics.cc 714 - Compare the base type with the assignment type. Throw an error if they are not the same if hard typed.
 - path.cc 24 - Set Locale - http://askubuntu.com/questions/236924/matlab-not-working
 - path.cc 78 - Throw Error