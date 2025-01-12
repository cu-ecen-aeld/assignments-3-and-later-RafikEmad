#!/bin/sh

# Check if the required arguments are passed
if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Error: Both directory path and search string must be provided."
  exit 1
fi

# Assign arguments to variables
filesdir="$1"
searchstr="$2"

# Check if the first argument is a valid directory
if [ ! -d "$filesdir" ]; then
  echo "Error: '$filesdir' is not a valid directory."
  exit 1
fi

# Initialize counters for files and matching lines
file_count=0
matching_lines=0

# Traverse the directory and its subdirectories
while IFS= read -r -d $'\0' file; do
  file_count=$((file_count + 1))  # Increment the file count

  # Count the lines in each file that match the search string
  matches_in_file=$(grep -c "$searchstr" "$file")
  matching_lines=$((matching_lines + matches_in_file))  # Increment the matching lines count
done < <(find "$filesdir" -type f -print0)

# Print the results
echo "The number of files are $file_count and the number of matching lines are $matching_lines"

exit 0