Functions
=========

Just like every language, Cobra includes functions. These are the components to functions:

 1. Arguments
 2. Function Name | Anonymus 
 3. Return values

Arguments
---------
Arguments can be of the following types:

 1. Bool
 2. Char
 3. Short
 4. Int
 5. Double
 6. Long
 7. String
 8. Array
 9. Function
 10. Object

Syntax
------


Declaration Examples
--------------------
```js
// Lazy declaration
add(a, b){
	return a + b;
}

var minus = (a, b){
	return a - b;
}

// Strong Type declaration
int add(a, b){
	return a + b;
}

int add = (a, b){
	return a + b;
}

string concat(a, b){
	return a + b;
}

// Strong Type declaration with strong type variables
int add(int a, int b){
	return a + b;
}

string concat(string a, string b){
	return a + b;
}

// Endless arguments
int add(int a, int b, int ...){
	int result = a + b;
	each (int arg in ...){
		result += arg;
	}
	return result;
}

// Type overides
[string, double, int] // restrict the overrides to only these
? add(? a, ? b){
	return a + b;
}

// Or more specifically
[
	add: string, int;
	a: double, int;
	b: double, int;
]
? add(? a, ? b){
	return a + b;
}

// To call these overriden functions
int total = add(1, 2);
double total = add(1, 2); // error

// Assigning the override to a var, it will pick 
// the first override option. In this case a string.
var total = add(1, 2);

// Sometimes we have different logic according to type
[
	add: string, int;
	a: double, int;
	b: double, int;
]
? add(? a, ? b){
	?double{ // specific for doubles
		return ((a + b) * 100) / 100;
	}
	?int{ // specific for ints
		return a + b;
	}

	// default. This only needs to be here if there
	// if there are other type blocks in the function
	?{ 
		return a + b;
	}
}


// Or you can just keep it generic
add(a, b){
	return a + b;
}
var total = add(1, 2);
```

Limited Usage Functions
-----------------------
Often times, a programmer will write a function that is really used only once, twice, or 10 times. After the usage, it's sits there to take up memory. To allow for programmer garbage collection control, we allow for self destroying functions.

**Example:**
```js

add(a, b){
	return a + b;
}[~1] // destroy the function after 1 use

minus(a, b){
	return a - b;
}[~10] // destroy the function after 10 uses
```