# Mini Java Toy Compiler

This compiler implements the most basic structures of a compiler for a esoteric and small language called MiniJava. The compiler consists of three main parts: **Lexical and Syntax analysis**, **Semantic analysis**, and finally, **Intermediate representation, Code generation, and Interpretation**.

**More info to come.**

## Building and Running the Compiler

The project is built using Make. The .exe, called "compiler", will be produced by simply running "make" or "make all". 

The program expects one argument which will be the file to compile and run. When a program has been run, "make CFG" will produce a Control Flow Graph that can be visually inspected. "make tree" will produce the abstract syntax tree that can also be visually inspected.

Each type of test, from the python test file, can be executed by running "make [test-type]_test". All tests can be run after each other by using "make test_all".
