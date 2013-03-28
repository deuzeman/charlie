charlie
=======

A small utility to check the integrity of ILDG configuration files.

This is a first draft implementation, for the moment without error handling and build system.

The code depends on the c-lime library, which will need to be available.

To compile, use:
gcc -std=c99 dml.c charlie.c -I/path_to_lime/include/ -L/path_to_lime/lib -llime -o charlie
