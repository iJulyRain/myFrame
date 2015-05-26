#!/bin/sh

echo ">>> make directory 'build'"
mkdir build

echo ">>> change directory 'build'"
cd build

echo ">>> cmake, build Makefile"
cmake ..

echo \#define WHO "`whoami`" > ../include/who.h

echo ">>> Make"
make
