#!/bin/bash


# compiles code
if [ ! -d "dist" ]; then
  gcc -o techshell src/techshell.c src/shelllib.c
else
  gcc -o dist/techshell src/techshell.c src/shelllib.c
fi

if [ $? -eq 0 ]; then
  echo "BUILD SUCCESSFUL"
  # if no dist folder, make it and move techshell into it
  if [ ! -d "dist" ]; then
    mkdir dist
    mv techshell dist/
  fi
  if [ $1 ]; then
    if [ $1 = "dev" ]; then
      ./dist/techshell
    else
      echo "Unrecognized command. Try \"dev\""
    fi
  fi
fi
