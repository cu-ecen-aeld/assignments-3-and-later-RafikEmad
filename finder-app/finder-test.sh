#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Siddhant Jajoo

set -e
set -u

NUMFILES=10
WRITESTR=AELD_IS_FUN
WRITEDIR=/tmp/aeld-data
username=$(cat /etc/finder-app/conf/username.txt)  # Update path for the username.txt

if [ $# -lt 3 ]
then
    echo "Using default value ${WRITESTR} for string to write"
    if [ $# -lt 1 ]
    then
        echo "Using default value ${NUMFILES} for number of files to write"
    else
        NUMFILES=$1
    fi    
else
    NUMFILES=$1
    WRITESTR=$2
    WRITEDIR=/tmp/aeld-data/$3
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"

# Remove any previous files or directories from previous runs
rm -rf "${WRITEDIR}"

# Create WRITEDIR if needed, only for assignment 2 and beyond (assignment1 is special)
assignment=`cat /etc/finder-app/conf/assignment.txt`  # Update path for the assignment.txt

if [ "$assignment" != 'assignment1' ]; then
    mkdir -p "$WRITEDIR"

    if [ -d "$WRITEDIR" ]; then
        echo "$WRITEDIR created"
    else
        echo "Failed to create directory $WRITEDIR"
        exit 1
    fi
fi

# Loop to write files using the compiled writer application instead of the shell script
for i in $(seq 1 $NUMFILES)
do
    writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"  # Use /usr/bin/writer
done

# Execute the finder.sh to search for the strings

OUTPUTSTRING=$(/usr/bin/finder.sh "$WRITEDIR" "$WRITESTR")  # Use /usr/bin/finder.sh

# Write the output to /tmp/assignment4-result.txt
echo ${OUTPUTSTRING} > /tmp/assignment4-result.txt

# Clean up temporary directory
rm -rf /tmp/aeld-data

set +e

# Check if the expected output matches
echo ${OUTPUTSTRING} | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
    echo "success"
    exit 0
else
    echo "failed: expected ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
    exit 1
fi