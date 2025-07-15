This document describes the expected format of the JSON payload accepted by the /solve endpoint or CSPScheduler::solve(...). All valid requests must conform to the constraints defined in schedule_contract.schema.json, based on JSON Schema Draft 2020-12.

## 📘 Overview
The payload represents a Constraint Satisfaction Problem (CSP) defined by:

* Variables with domains
* Constraints between those variables
* An optional objective for optimization
* Optional metadata
* Optional callback URL for delivering async results

✍️ JSON Format: Top-Level Fields
🔹 variables (required)
An array of variable definitions. Each variable must have:

Field	Type	Description
name	string	Unique variable name
type	string	"int" or "bool"
domain	object	Must have one of: interval (2-tuple of ints) or values (list of ints)
Example:

```
{
  "name": "TaskA",
  "type": "int",
  "domain": {
    "interval": [0, 10]
  }
}
```
or
```
{
  "name": "Flag",
  "type": "bool",
  "domain": {
    "values": [0, 1]
  }
}
```
🔹 constraints (required)
An array of constraint definitions. Each item must include:

Field	Type	Description
type	string	Type of constraint (see below)
variables	string[]	Names of involved variables
Supported Constraint Types
all_different

* linear
* allowed_assignments
* forbidden_assignments
* implication
* element
* not_equal
* equal
* greater_than
* less_than

Depending on type, additional fields like value, relation, or coefficients may be required.

Example:

```
{
  "type": "all_different",
  "variables": ["A", "B", "C"]
}
```
or 
```
{
  "type": "linear",
  "variables": ["x", "y"],
  "coefficients": [3, -1],
  "relation": "greater_than",
  "value": 10
}
```
🔹 objective (optional)
Defines an optimization objective over the problem’s variables.

```
"objective": {
  "sense": "maximize",
  "expression": {
    "coefficients": {
      "x": 5,
      "y": 3
    }
  }
}
```
Field	Type	Description
sense	string	"maximize" or "minimize"
expression.coefficients	object	Map of variable names to numeric coefficients
🔹 callback_url (optional)
If provided, results may be sent via HTTP POST to this URL.

TBD
"callback_url": "https://your-app.com/api/callback"
🔹 metadata (optional)
Free-form dictionary. Ignored by the solver but passed through to the response for traceability.

json
"metadata": {
  "job_id": "abc-123",
  "user": "client42"
}
✅ Schema Compliance
Your payload must conform to the official JSON schema. You can validate it using json-schema tools or via the isJsonValidatedToSchema() function provided by the engine.

💡 Restrictions
variables and constraints are required

Additional top-level properties are not allowed

Each constraint type must follow its required shape

At least one variable is required for the model to be meaningful

📚 Schema Metadata
```
"$schema": "https://json-schema.org/draft/2020-12/schema",
"$id": "https://your-domain.com/schema/schedule_contract.schema.json",
"title": "SchedulingProblemRequest"
```

🧪 Example Input
```
{
  "variables": [
    { "name": "A", "type": "int", "domain": { "interval": [1, 3] } }
  ],
  "constraints": [
    { "type": "equal", "variables": ["A"], "value": 2 }
  ]
}
```

🔗 Resources
JSON Schema Draft: https://json-schema.org/draft/2020-12/schema

Contract source: schedule_contract.schema.json