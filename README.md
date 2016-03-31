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
If you would like to contribute to the CobraLang project, pull requests are always welcome. The folder structure goes as follows:
 - **src** (source code)
  - **ast** (abstract syntax tree related files)
  - **error** (error files)
  - **mem** (memory related files)
  - **parser** (the basic parser from code -> ast -> syntax analysis)
  - **scanner** (the lexical analysis)
  - **semantics** (the semantics analysis)
  - **token** (the token related files, see scanner)
  - **types** (intended for all internal .cb files and the interface with c++)
    - **array** (internal array files)
    - **script** (internal scripting, this is the backend how the string is compiled into the code)
    - **string** (internal string files)
    - **try_catch** (internal try_catch files)
    - **vector** (internal vector files)

Todo:
------
 - semantics.cc 236 - add variable types
 - semantics.cc 414 - Compare the base type with the assignment type. Throw an error if they are not the same if hard typed.