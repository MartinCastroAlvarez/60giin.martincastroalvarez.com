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

# Check for assimp
if [[ "$OSTYPE" == "darwin"* ]]; then
  if ! brew ls --versions assimp > /dev/null 2>&1; then
    echo "assimp not found. Installing via brew..."
    brew install assimp
  fi
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
  if ! dpkg -s libassimp-dev > /dev/null 2>&1; then
    echo "assimp not found. Installing via apt-get..."
    sudo apt-get update && sudo apt-get install -y libassimp-dev
  fi
fi

# Build project
pwd 
cmake ../src/

# Find project
if [ "$1" == "" ]
then
  echo "Usage: $0 <project-number>"
fi

PROJECT=$(ls -d ../src/projects/${1}* | xargs -I{} basename {})
echo $PROJECT
if [ $(echo "$PROJECT" | wc -l) != 1 ] 
then
  echo "ERROR: Failed to find project"
fi

make $PROJECT

# Execute project
./$PROJECT