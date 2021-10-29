# TRegex

This is regex library that I make in my free time to learn graph
It's still not complete (and pretty sure it has many bugs)

- Could decode notation to graph, symbols that are already implemented (*, (), |, +)
- Could compare string if it's match with pattern

## Struct
1. TRDArray
	- struct for graph node and containing dynamic array for next state
	- Must always used it as pointer
2. TRegex
	- Struct for regex
	- Will delete all node when change the regex notation
	- Could compile regex notation to graph

Don't use this library in your program, this is still not complete and buggy, instead use already built-in library like regex.h in C
Hope you have a nice day! ꒰(･‿･)꒱