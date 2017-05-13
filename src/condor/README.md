# Core
The core is not exposed to the public. The code in the following folder is wrapped by a "internal" namespace.

## Isolate
An isolate is an isolated instance of Condor. All memory management will be isolated to an Isolate.

## Memory Management
Allocating new memory is slow because it goes through the client OS. The [Experimental](https://github.com/CondorLang/Condor/tree/master/experimental/c) branch implements the language solely using stack memory. However, the next best option is to allocate a large chunk of memory, then reuse portions of that memory. The garbage collector is tasked with cleaning the unused memory.

Most object will be allocated using the following method:
```
ObjectName rawObject = ObjectName::New(isolate);
```
