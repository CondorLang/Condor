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
