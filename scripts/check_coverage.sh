#!/bin/bash

# Set the exit status to 0 (success) initially
exit_status=0

# Generate coverage report using gcovr and store the output in a variable
coverage_report=$(gcovr --print-summary --exclude "test/main.*")

# Extract the line with "Lines: X.X%" from the coverage report using grep
coverage_line=$(echo "$coverage_report" | grep -E "lines:[[:space:]]*100\.0%")

# Check if the coverage_line is empty (i.e., coverage is not 100%)
if [ -z "$coverage_line" ]; then
  echo "Test coverage is not 100%."
  exit_status=1
else
  echo "Test coverage is 100%."
fi

# Exit with the appropriate status
exit $exit_status