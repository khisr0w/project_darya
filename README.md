# Project Darya (v0.01)
An interpreted programming language (in alpha stage) made specifically for the purpose of teaching programming/data science to students of Non-English communities.
Targeted specifically towards Afghan school kids.\
The Project is written **entirely from scratch** with no external libraries involved, even C runtime library functions are not included in the code.

## Examples
Example code to find the minimum number
![Example code to find the minimum number](/msc/example_min.jpg)
Gradient descent of a simple loss function $f(x) = x^2 + x + 1$  
![Gradient descent of a simple loss function](/msc/example_gradient_descent.jpg)
## Notice
The project is still in heavy development and so inexperienced users are discouraged to use it as their de-facto programming exercise.\
If you are an experienced C/C++ developer, with knowledge of programming language development, we can work together to finish the project faster.\

## Features in development
- The language will consist of a **Translation Layer** capable to translating UTF-8 compliant language instructions into the native ANSI code of the interpreter.
- The code for the translation layer will be modular so that anyone can use the interperter and tranform it to recongize any UTF-8 compliant text as a viable instruction.
- (Still cooking) The language will have a binding to C/C++ programming language that would allow it to utilize the native platform funcitionalities for heavy-computations, such as rendering graphics and numerial calcualtions in data science applications.
- Binding to the [numpy](https://github.com/numpy/numpy) library, entirely written in C/C++
- Ability to build executables(exe/dmg/app/apk) that would run independently of the interpreter.
- Making of a standalone interpreter for ARM NEON architecture that would be deployed to android mobile phones as well as Rasberry Pi (Raspbian/Linux)

## TODOs
- More Precise floating point conversion from string to float and vice versa
- Robust power operation; handle wrapping around 32 bit and scientific notation of big numbers
- MUST decide whether a fixed starting memory is a good idea for this or not?
- On the spot conversion of int to float and vice versa when the value precision allows it
- MUST Support operator overloading
- Make scope for the conditional, loop and function calls
- Custom built function support within the language

## WARNINGs
- Power operation is unstable and possibly unusable at this point and must fixed!
- Float-String conversion is extremely unstable and must be amended with a better algorithm
- Clean up after each statement is done, and the blocks must be freed after each 5 statements
