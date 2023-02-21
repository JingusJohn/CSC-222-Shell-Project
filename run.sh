#!/bin/bash


# compiles code then runs if no compilation errors
gcc -o techshell techshell.c shelllib.c
if [ $? -eq 0 ]; then
  ./techshell
fi
