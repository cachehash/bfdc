# bfdc
This brainfuck compiler is a bfd... or at least it thinks of itself that way. The main emphasis of bfdc is speed, but it tries to provide features that don't bog it down too much.

# Overview
bfdc is an optimizing compiler/interpreter. This project emphasizes speed so optimizations are done before the interpreter is invoked and there is little effort to check if the program being interpreted has stepped outside of the legal memory region. However if this causes a crash the interpreter will let you know, not the compiled code however.

# Usage

```Text
Usage: ./bfdc [OPTION]... FILE

   -i, --interpret              interpret the brainfuck code specified in FILE
   -t, --traverse               interpret the parse tree used for compilation instead of compiling
   -j, -d, --jit, --dynarec     use dynamic recompilation to run the program
   -o, --output=FILE            output name for the compiled brainfuck
   -O, --optimize=NUM           optimization level
   -c, --num-cells=NUM          number of cells to use
   -C, --cell-type=TYPE         data type to use for cells
   -E, --eof=TYPE               format for EOF to use
   -h, --help                   display this help text

Examples:
   ./bfdc -i hello.b            interpret the contents of hello.b
   ./bfdc hello.b -o hello.c    compile hello.b and output hello.c
```
