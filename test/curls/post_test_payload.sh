#!/bin/bash

# Run this from within curls dir
# Usage: ./post_json.sh <filename> <endpoint>
# Example: ./post_json.sh hard0.json http://localhost:8080/solve

if [ $# -ne 2 ]; then
    echo "Usage: $0 <filename> <endpoint>"
    exit 1
fi

FILENAME="$1"
ENDPOINT="$2"

curl -X POST -H "Content-Type: application/json" -d @"../test_payloads/$FILENAME" "$ENDPOINT"