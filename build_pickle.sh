#!/bin/bash

make ARCH=x86_64 clean -j8
make ARCH=x86_64 -j8
# make ARCH=x86_64 LLVM=/usr/lib/llvm-20/bin/
