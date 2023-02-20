# xTea

**A naive XTEA (eXtended TEA) implementation.**

A Cpp implementation of the Xtea block cipher cryptographic algorithms.

It's based on the original implementation by David Wheeler and Roger Needham.

I wrote it for a friend that should have written it for an University exam some years ago.
It works both in standard "plain" mode and in the more secure CBC (Cipher Block Chaining) mode.

**DO NOT USE THIS LIBRARY FOR ANYTHING SERIOUS, IT'S JUST A TOY**

## Compiling

Just use the makefile, or cmake.

![C/C++ CI](https://github.com/VincenzoLaSpesa/xTea/workflows/C/C++%20CI/badge.svg)

## Why?

Currently I mostly use this repository for experimenting with GitHub actions and package management in c++.

## Conan package

This repository contains a conan package and a test package for it.
For creating the package use something like `conan create . demo/testing` that will build the package and copy it to the local cache ( `~/.conan/` usually) using the current default toolchain


### Conan references

-   https://docs.conan.io/en/latest/creating_packages/package_repo.html

## License

This software is released under the MIT License 


    MIT License
    Copyright (c) 2017 Vincenzo La Spesa
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.