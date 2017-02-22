# Experiemental C Creation
The purpose behind this part of the library is to optimize speed. When using the Malloc call, 
it takes about 3,000% more time than just using the stack. For this reason, we are developing this to 
See how close we can get to the nanoseconds as possible. Avoiding Malloc also allows for the compiler
to be portable for any operating system.

## Building
```
./init [-d|--debug] [-e|--execute] [-c|--clean] [-n|--namespace fileName] [-t|--test]
```

### Arguments
 - Debug: Initiates and runs all the debug prints throughout the code. These could be in any file. Due to the exhaustive amount of debug calls, we created a namespace to filter
 - Namespace: The file name to filter the debugs
 - Execute: Execute after building
 - Clean: Clean before building
 
## Rules
 - Do not use Malloc unless absolutely needed
 - Do not implement a file retriever function. Only char* can be passed directly
 - Always run ./mem which runs Valgrind. No memory leaks allowed.

<a href="https://koding.com/"> <img src="http://koding-cdn.s3.amazonaws.com/badges/made-with-koding/v1/koding_badge_RectangleLight.png" srcset="http://koding-cdn.s3.amazonaws.com/badges/made-with-koding/v1/koding_badge_RectangleLight.png 1x, http://koding-cdn.s3.amazonaws.com/badges/made-with-koding/v1/koding_badge_RectangleLight@2x.png 2x" alt="Made with Koding" /> </a>
