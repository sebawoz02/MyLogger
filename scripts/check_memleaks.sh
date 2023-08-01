#!/bin/bash

# Run program with Valgrind, redirecting the output to a log file
valgrind --leak-check=full --error-exitcode=1 ./test.out 2> valgrind_log.txt
rm -rf valgrind_log.txt
# Check the exit code of Valgrind
valgrind_exit_code=$?

# If Valgrind's exit code is 1, it means there were memory leaks or errors
if [ $valgrind_exit_code -eq 1 ]; then
  echo "Memory leaks or errors detected by Valgrind. Check 'valgrind_log.txt' for details."
  exit 1
else
  echo "No memory leaks detected by Valgrind."
  exit 0
fi