# xTea
A toy implementation of the xTea algorithm

## A naive XTEA (eXtended TEA) implementation.

A Cpp implementation of the Xtea block cipher cryptographic algorithms.

It's based on the original implementation by David Wheeler and Roger Needham.

I wrote it for a University exam some years ago.
It works both in standard "plain" mode and in the more secure CBC (Cipher Block Chaining) mode.


### Compiling

The trivial way to compile the files and obtain an executable, is by running the command:

g++ main.cpp functions.cpp xTea.cpp -o xTea.bin

you can also use the makefile

