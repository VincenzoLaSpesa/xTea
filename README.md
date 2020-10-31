# xTea

## A naive XTEA (eXtended TEA) implementation.

A Cpp implementation of the Xtea block cipher cryptographic algorithms.

It's based on the original implementation by David Wheeler and Roger Needham.

I wrote it for a friend that should have written it for an University exam some years ago.
It works both in standard "plain" mode and in the more secure CBC (Cipher Block Chaining) mode.

### Compiling

Just

g++ main.cpp main.cpp xTea.cpp -o xTea

or use the makefile

![C/C++ CI](https://github.com/VincenzoLaSpesa/xTea/workflows/C/C++%20CI/badge.svg)

### Why?

Currently I mostly use this repository for experimenting with GitHub actions.