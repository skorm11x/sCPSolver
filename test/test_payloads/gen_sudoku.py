import json

puzzle = [
    [0,0,0, 0,0,0, 6,0,8],
    [0,0,8, 9,0,0, 0,0,0],
    [0,0,0, 2,5,0, 0,0,0],
    [0,0,0, 0,0,1, 0,2,0],
    [0,7,0, 0,0,0, 0,0,0],
    [0,0,0, 0,0,0, 0,0,9],
    [0,0,0, 0,0,0, 9,0,0],
    [0,0,0, 0,0,0, 0,7,0],
    [0,0,0, 0,0,4, 0,0,0],
]

variables = []
for r in range(9):
    for c in range(9):
        cell = puzzle[r][c]
        name = f"r{r+1}c{c+1}"
        if cell == 0:
            domain = {"values": list(range(1,10))}
        else:
            domain = {"values": [cell]}
        variables.append({"name": name, "type": "int", "domain": domain})

constraints = []

for r in range(9):
    constraints.append({
        "type": "all_different",
        "variables": [f"r{r+1}c{c+1}" for c in range(9)]
    })

for c in range(9):
    constraints.append({
        "type": "all_different",
        "variables": [f"r{r+1}c{c+1}" for r in range(9)]
    })

for br in range(3):
    for bc in range(3):
        box_vars = []
        for dr in range(3):
            for dc in range(3):
                r = br*3 + dr
                c = bc*3 + dc
                box_vars.append(f"r{r+1}c{c+1}")
        constraints.append({
            "type": "all_different",
            "variables": box_vars
        })

csp = {
    "variables": variables,
    "constraints": constraints
}

with open("sudoku_csp.json", "w") as f:
    json.dump(csp, f, indent=2)