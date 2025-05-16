#!/bin/bash

###############################################################################
# generate_file.sh
# 
# Description:
#   Generates a file of a specified size and content type.
#   Supports KB, MB, and GB units (e.g., 0.5GB, 100MB, 512KB).
#
# Usage:
#   ./generate_file.sh <size_with_unit> <filename> [type]
#
# Parameters:
#   <size_with_unit>  - The desired size (e.g., 0.5GB, 100MB, 512KB)
#   <filename>        - The name of the file to generate
#   [type]            - (Optional) File content type:
#                       • zero   - Filled with zero bytes (default)
#                       • random - Filled with random data
#                       • sparse - Sparse file using truncate
#
# Examples:
#   ./generate_file.sh 100MB test_zero.bin
#   ./generate_file.sh 0.5GB test_random.bin random
#   ./generate_file.sh 512KB test_sparse.img sparse
#
###############################################################################

if [ "$#" -lt 2 ]; then
  echo "❌ Error: Not enough arguments."
  echo
  grep '^#' "$0" | sed 's/^#//'
  exit 1
fi

SIZE_INPUT=$1
FILENAME=$2
TYPE=${3:-zero}

# Extract number and unit
NUM=$(echo "$SIZE_INPUT" | grep -Eo '^[0-9]+([.][0-9]+)?')
UNIT=$(echo "$SIZE_INPUT" | grep -Eo '[KMG]B' | tr '[:lower:]' '[:upper:]')

# Default to GB if no unit
if [ -z "$UNIT" ]; then
  UNIT="GB"
fi

# Validate number format
if ! [[ "$NUM" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
  echo "❌ Error: Invalid size format."
  exit 1
fi

# Convert to bytes
case "$UNIT" in
  KB)
    SIZE_BYTES=$(echo "$NUM * 1024" | bc | awk '{printf "%d\n", $0}')
    ;;
  MB)
    SIZE_BYTES=$(echo "$NUM * 1024 * 1024" | bc | awk '{printf "%d\n", $0}')
    ;;
  GB)
    SIZE_BYTES=$(echo "$NUM * 1024 * 1024 * 1024" | bc | awk '{printf "%d\n", $0}')
    ;;
  *)
    echo "❌ Error: Unsupported unit '$UNIT'. Use KB, MB, or GB."
    exit 1
    ;;
esac

echo "📁 Creating file: $FILENAME"
echo "📦 Size: $NUM $UNIT = $SIZE_BYTES bytes"
echo "🧾 Type: $TYPE"

case "$TYPE" in
  zero | random)
    SRC="/dev/zero"
    [ "$TYPE" = "random" ] && SRC="/dev/urandom"

    # Determine block size and count
    if [ "$SIZE_BYTES" -lt 1048576 ]; then
      BLOCK_SIZE=1
      COUNT=$SIZE_BYTES
    else
      BLOCK_SIZE=1048576
      COUNT=$(echo "$SIZE_BYTES / $BLOCK_SIZE" | bc)
    fi

    dd if="$SRC" of="$FILENAME" bs=$BLOCK_SIZE count=$COUNT status=progress
    ;;
  sparse)
    truncate -s "$SIZE_BYTES" "$FILENAME"
    ;;
  *)
    echo "❌ Error: Unknown type '$TYPE'. Use: zero, random, sparse."
    exit 1
    ;;
esac

echo "✅ File '$FILENAME' created successfully."

