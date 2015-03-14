Include
======

Include is used when including another file into your project. Include will be search, compiled and executed before the rest of the code is executed. (See [export](export.md)) The included modules will return the exported variables.

Syntax
------
**Single inline declaration** in the following order:

 1. Keyword: "include"
 2. Open String
 3. File path
 4. Close String
 5. Semicolon

**Block declaration** in the following order:
 1. Keyword: "include"
 2. Left Brace
 3. Open String
 4. File path
 5. Close String
 6. Semi Colon
 7. Repeat 3-6
 8. Right Brace


Examples
--------
```js
var ajax = include 'ajax.cb';
```

```js
var bulk = include {
	'ajax.cb';
	'http.cb';
	'html.cb';
}
```