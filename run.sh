#!/bin/bash

clear

mkdir ./build/ 2>/dev/null
cd ./build/

# Check if cmake is installed
cmake --version
if [ "$?" != "0" ]
then
  brew install cmake
fi
cmake --version
if [ "$?" != "0" ]
then
  echo "ERROR: cmake not found"
  exit 1
fi

pwd 
cmake ../src/
