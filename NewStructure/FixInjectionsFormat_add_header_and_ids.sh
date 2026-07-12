#!/bin/bash

# Ensure a file path argument was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

FILE="$1"

# Check if the target file actually exists
if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not found."
    exit 1
fi

# Check if the first line already starts with #ch
if head -n 1 "$FILE" | grep -q "^#ch"; then
    echo "File already contains the '#ch' header. No changes made."
else
    # Define the precise header requested
    HEADER="#ch,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6"
    
    # Create a secure temporary file to stage the updates safely
    TMP_FILE=$(mktemp)
    
    # 1. Write the new header to the temporary file
    echo "$HEADER" > "$TMP_FILE"
    
    # 2. Prepend the running number (using awk's NR variable) followed by a comma to each original line
    awk '{print NR "," $0}' "$FILE" >> "$TMP_FILE"
    
    # 3. Safely replace the original file with the modified one
    mv "$TMP_FILE" "$FILE"
    
    echo "Successfully added running channel IDs and header to '$FILE'."
fi