# CobraLang [![Build Status][travis-image]][travis-url]

CobraLang is intended to be another simple and intuitive programming language. The syntax is familiar by design, but allows for the combining of a few concepts.

See [Wiki](https://github.com/chaseWillden/CobraLang/wiki) for documentation.

Top Problems with Programming
-----------------------------
 - Lack of Code organization
 - Lack of Simplicity
 - Lack of Readability
 - (Java/C#) Not too verbose
 - Too much Memory Management

Building Instructions
---------------------
For the first initial download, you'll need to run, make clean to ensure that all the *.d files are removed.

Building should be fairly easy. Required programs:
 - gcc

For building the CobraLang .a library
```
make all
```
To build all and compile the main.cc found in test/main.cc
```
make || make mt
```
To build and perform a memory test, requires [valgrind](http://valgrind.org/) to be installed
```
make mem
```
To clean all object files
```
make clean
```
To debug using terminal, requires [gdb](https://www.gnu.org/software/gdb/) to be installed
```
make d
```

[travis-url]: https://travis-ci.org/chaseWillden/CobraLang/
[travis-image]: https://img.shields.io/travis/chaseWillden/CobraLang/master.svg?style=flat

Contributing:
-------------
See [Contributing](https://github.com/chaseWillden/CobraLang/wiki/Contributing) page in the wiki

Todo:
------
 - node.h 22 - Create a Expand() function which expands the node and the tree
 - execute.cc 50 - Erase values in all non return locals
 - execute.cc 168 - Set row and col for tracking
 - execute.cc 169 - String, Char, and other data types need to be implemented
 - execute.cc 281 - What about twice?
 - execute.cc 357 - Implement setting object variables
 - execute.cc 358 - Implement bitwise
 - execute.cc 576 - Attach the correct parent scope to the scope
 - parser.cc 370 - Account for variable declarations
 - parser.cc 794 - Disallow for allow-native on a non internal file
 - scanner.cc 462 - Include Octal, Hex, UTF-8, UTF-16, see https://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
 - internal.cc 24 - replace with a macro
 - semantics.cc 310 - add variable types
 - semantics.cc 335 - Fix RemoveAllAfter in scope. 
 - semantics.cc 514 - Compare the base type with the assignment type. Throw an error if they are not the same if hard typed.