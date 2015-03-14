Array
======
Arrays are nothing special. They are simular to javascript, however, it can be stronger typed.


Examples
--------
```js
var array = [1, 2, 3, 4];
var mixedArray = [1, 2, 'a', 'apple'];

array.push(5);
var idx = array.indexOf(1);

// A stronger typed array
int ary = [1, 2, 3, 4, 5];
string ary = ['a', 'b', 'c', 'd'];
bool ary = [false, true, 1, 'a'] // error;
```

Features
--------
**Call All**

```js
method1(){
	// stuff
}
method2(){
	// stuff
}

var ary = [method1, method2];
ary.callAllFunctions();
```