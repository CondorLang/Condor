Export
======

Export is used specifically if this module is a library of some sort. When using [include](include.md) followed by a filename, the file will be compiled and executed prior to the rest of the file. Export is a publically accessable object. When using export on a variable, the variable is then made available to be used whenever that file is included in another. Exports will be disreguarded unless called by an include.

Syntax
------
**Single inline declaration** in the following order:

 1. Keyword: "export"
 2. Variable|Object name
 3. Semicolon

**Block declaration** in the following order:
 1. Keyword: "include"
 2. Left Brace
 3. Variable|Object name
 4. Semi Colon
 5. Repeat 3-4
 6. Right Brace


Examples
--------
```js
var pi = 3.14159265;
export pi;
```

```js
var pi = 3.14159265;
add(a, b){
	return a + b;
}
export {
	pi;
	add;
}
```