#!/bin/bash

# Function to check if a command is installed
function check_command {
  command -v "$1" >/dev/null 2>&1
}

# Check and install Valgrind
if ! check_command valgrind; then
  echo "Valgrind is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install valgrind
fi

# Check and install Python 3
if ! check_command python3; then
  echo "Python 3 is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install python3
fi

# Check and install python3-pip
if ! check_command pip3; then
  echo "python3-pip is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install python3-pip
fi

# Check and install GCC
if ! check_command gcc; then
  echo "GCC is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install gcc
fi

# Check and install Clang
if ! check_command clang; then
  echo "Clang is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install clang
fi

# Check and install Git
if ! check_command git; then
  echo "Git is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install git
fi

# Check and install gcovr
if ! check_command gcovr; then
  echo "gcovr is not installed. Installing..."
  sudo pip3 install gcovr
fi

echo "All required dependencies are installed."