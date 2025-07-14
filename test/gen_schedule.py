import json
import random

NUM_EMPLOYEES = 2000 #20, 200
NUM_DAYS = 31 #14, 30
NUM_SHIFTS = 40 #3, 4
EMPLOYEES_PER_SHIFT = 4 #2, 4
total_assignments = NUM_DAYS * NUM_SHIFTS * EMPLOYEES_PER_SHIFT
MIN_SHIFTS = total_assignments // NUM_EMPLOYEES

employees = [f"E{e+1}" for e in range(NUM_EMPLOYEES)]
days = [f"D{d+1}" for d in range(NUM_DAYS)]
shifts = [f"S{s+1}" for s in range(NUM_SHIFTS)]

schedule = [[[0 for s in range(NUM_SHIFTS)] for d in range(NUM_DAYS)] for e in range(NUM_EMPLOYEES)]

for d in range(NUM_DAYS):
    for s in range(NUM_SHIFTS):
        available_emps = list(range(NUM_EMPLOYEES))
        random.shuffle(available_emps)
        chosen = []
        for e in available_emps:
            if sum(schedule[e][d]) == 0:
                max_consecutive = 6
                cons = 0
                for prev in range(d-1, -1, -1):
                    if any(schedule[e][prev]):
                        cons += 1
                    else:
                        break
                if cons < max_consecutive:
                    chosen.append(e)
                    schedule[e][d][s] = 1
                    if len(chosen) == EMPLOYEES_PER_SHIFT:
                        break
        if len(chosen) < EMPLOYEES_PER_SHIFT:
            for e in available_emps:
                if e not in chosen and sum(schedule[e][d]) == 0:
                    chosen.append(e)
                    schedule[e][d][s] = 1
                    if len(chosen) == EMPLOYEES_PER_SHIFT:
                        break

variables = []
var_names = []
for e_idx, e in enumerate(employees):
    for d_idx, d in enumerate(days):
        for s_idx, s in enumerate(shifts):
            name = f"{e}_{d}_{s}"
            var_names.append(name)
            variables.append({
                "name": name,
                "type": "bool",
                "domain": {"values": [0, 1]}
            })

constraints = []

for d in days:
    for s in shifts:
        shift_vars = [f"{e}_{d}_{s}" for e in employees]
        constraints.append({
            "type": "linear",
            "variables": shift_vars,
            "coefficients": [1] * NUM_EMPLOYEES,
            "relation": "equal",
            "value": EMPLOYEES_PER_SHIFT
        })

for e in employees:
    for d in days:
        emp_day_vars = [f"{e}_{d}_{s}" for s in shifts]
        constraints.append({
            "type": "linear",
            "variables": emp_day_vars,
            "coefficients": [1] * NUM_SHIFTS,
            "relation": "less_than",
            "value": 2
        })

for e in employees:
    for start in range(NUM_DAYS - 6 + 1):
        window_days = days[start:start+6]
        window_vars = [f"{e}_{d}_{s}" for d in window_days for s in shifts]
        constraints.append({
            "type": "linear",
            "variables": window_vars,
            "coefficients": [1] * (6 * NUM_SHIFTS),
            "relation": "less_than",
            "value": 16
        })

for e in employees:
    emp_vars = [f"{e}_{d}_{s}" for d in days for s in shifts]
    constraints.append({
        "type": "linear",
        "variables": emp_vars,
        "coefficients": [1] * (NUM_DAYS * NUM_SHIFTS),
        "relation": "greater_than",
        "value": MIN_SHIFTS - 1
    })

fixed_assignments = []
for e_idx, e in enumerate(employees):
    for d_idx, d in enumerate(days):
        for s_idx, s in enumerate(shifts):
            if schedule[e_idx][d_idx][s_idx] == 1 and random.random() < 0.10:
                fixed_assignments.append({
                    "type": "equal",
                    "variables": [f"{e}_{d}_{s}"],
                    "value": 1
                })
constraints.extend(fixed_assignments)

objective_coeffs = {name: 1 for name in var_names}

problem = {
    "variables": variables,
    "constraints": constraints,
    "metadata": {
        "description": "Simplified hard but feasible employee scheduling: 20 employees, 14 days, 3 shifts/day, tight coverage, max consecutive workdays, min shifts, some fixed assignments."
    },
    "objective": {
        "sense": "minimize",
        "expression": {
            "coefficients": objective_coeffs
        }
    }
}

with open("hard_feasible_schedule.json", "w") as f:
    json.dump(problem, f, indent=2)

print("Generated hard_feasible_schedule.json")
