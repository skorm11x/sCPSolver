import json
import sys
import os
import math
import argparse

def sudoku_to_ortools_json(puzzle):
    n = len(puzzle)
    variables = []
    for r in range(n):
        for c in range(n):
            cell = puzzle[r][c]
            name = f"r{r+1}c{c+1}"
            domain = {"values": list(range(1, n+1))} if cell == 0 else {"values": [cell]}
            variables.append({"name": name, "type": "int", "domain": domain})

    constraints = []
    for r in range(n):
        constraints.append({
            "type": "all_different",
            "variables": [f"r{r+1}c{c+1}" for c in range(n)]
        })
    for c in range(n):
        constraints.append({
            "type": "all_different",
            "variables": [f"r{r+1}c{c+1}" for r in range(n)]
        })

    box_size = int(math.isqrt(n))
    if box_size * box_size == n:
        for br in range(box_size):
            for bc in range(box_size):
                box_vars = []
                for dr in range(box_size):
                    for dc in range(box_size):
                        r = br * box_size + dr
                        c = bc * box_size + dc
                        box_vars.append(f"r{r+1}c{c+1}")
                constraints.append({
                    "type": "all_different",
                    "variables": box_vars
                })
    return {"variables": variables, "constraints": constraints}

def load_puzzle_from_file(filename):
    ext = os.path.splitext(filename)[-1].lower()
    if ext == '.json':
        with open(filename) as f:
            return json.load(f)
    elif ext == '.csv':
        import csv
        with open(filename) as f:
            reader = csv.reader(f)
            return [[int(cell) for cell in row] for row in reader]
    else:
        with open(filename) as f:
            lines = [line.strip() for line in f if line.strip()]
            return [[int(ch) for ch in line if ch.isdigit()] for line in lines]

def load_puzzle_from_string(s):
    s = ''.join([ch for ch in s if ch.isdigit()])
    n = int(len(s) ** 0.5)
    if n * n != len(s):
        raise ValueError("Input string does not represent a square puzzle (length is not a perfect square).")
    return [[int(s[i * n + j]) for j in range(n)] for i in range(n)]

def main():
    parser = argparse.ArgumentParser(description="Convert a Sudoku puzzle to OR-Tools JSON CSP format.")
    parser.add_argument("input", help="Input file (.json, .csv, .txt) or digit string")
    parser.add_argument("-o", "--output", help="Output JSON file", default="sudoku_csp.json")
    args = parser.parse_args()

    if os.path.isfile(args.input):
        puzzle = load_puzzle_from_file(args.input)
    else:
        puzzle = load_puzzle_from_string(args.input)

    csp = sudoku_to_ortools_json(puzzle)
    with open(args.output, "w") as f:
        json.dump(csp, f, indent=2)
    print(f"Sudoku CSP written to {args.output}")

if __name__ == "__main__":
    main()
