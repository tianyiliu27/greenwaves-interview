#!/bin/bash

while getopts "s:" opt; do
  case ${opt} in
    s)
      size=${OPTARG}
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

if [[ -z $size ]]; then
  echo "Missing size option (-s)"
  exit 1
fi

# Perform desired operations with the size value
echo "Size: $size"

# Execute your desired commands using the $size variable

