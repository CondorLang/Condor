<!-- ![Condor Title](https://github.com/CondorLang/Condor/blob/master/Condor%20Title.png) -->
# Condor [![Build Status][travis-image]][travis-url]

Condor is intended to be another simple and intuitive programming language. The syntax is familiar by design, but allows for the combining of a few concepts.

Documentation
-------------
See [Wiki](https://github.com/CondorLang/Condor/wiki) for documentation.

Contributing:
-------------
See [Contributing](https://github.com/CondorLang/Condor/wiki/Contributing) page in the wiki
See [Developers](https://github.com/CondorLang/Condor/wiki#developers) for building instructions.

[travis-url]: https://travis-ci.org/CondorLang/Condor/
[travis-image]: https://img.shields.io/travis/CondorLang/Condor/master.svg?style=flat

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

## Building Instructions
CondorLang uses CMake to generate the Makefiles. The following commands build the project.
### Build
* ```make build``` - Builds the Condor program
* ```make clean``` - Cleans all the object files

## Running CondorLang
The run CondorLang, use the following commands.
```./build/condor [file]```

Todo:
------
 - semantics.cc 167 - recursive
 - semantics.cc 305 - Verify this is ok and always true
 - semantics.cc 688 - Is this true always?
 - semantics.cc 730 - Compare the base type with the assignment type. Throw an error if they are not the same if hard typed.
 - semantics.cc 763 - Incomplete crawling of all types.
 - internal.cc 140 - Throw Error
 - internal.cc 152 - Throw Error
 - internal.cc 164 - Throw Error
 - internal.cc 309 - Implement thread sleep
 - internal.cc 347 - Move to a macro
 - execute.cc 231 - Set row and col for tracking
 - execute.cc 480 - Implement null, undefined, and other types
 - execute.cc 511 - Implement bitwise
 - parser.cc 913 - Disallow for allow-native on a non internal file
 - path.cc 20 - Set Locale - http://askubuntu.com/questions/236924/matlab-not-working
 - path.cc 88 - Throw Error
 - scanner.cc 482 - Include Octal, Hex, UTF-8, UTF-16, see https://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
 - node.h 28 - Create a Expand() function which expands the node and the tree