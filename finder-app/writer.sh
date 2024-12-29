#!/bin/bash

# Check if the required arguments are passed
if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Error: Both file path and text string must be provided."
  exit 1
fi

# Assign arguments to variables
writefile="$1"
writestr="$2"

# Create the directory path if it doesn't exist
dirpath=$(dirname "$writefile")
if [ ! -d "$dirpath" ]; then
  echo "Error: Directory '$dirpath' does not exist. Creating it..."
  mkdir -p "$dirpath" || { echo "Error: Failed to create directory."; exit 1; }
fi

# Write the string to the file (overwrite if it exists)
echo "$writestr" > "$writefile" || { echo "Error: Failed to create or write to the file."; exit 1; }

echo "File '$writefile' created with content: $writestr"
exit 0
