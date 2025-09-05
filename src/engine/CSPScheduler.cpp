#include "CSPScheduler.h"
#include <ortools/sat/cp_model.h>
#include <ortools/sat/cp_model.pb.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace operations_research;
using namespace sat;
using namespace nlohmann;
using namespace std;

//TODO: explicitly define return JSON formats
/**
 * Solve a constraint satisfaction problem (CSP) defined in JSON.
 *
 * This function accepts a validated JSON object conforming to the
 * CSP schema. It encodes the problem into an OR-Tools CP-SAT model,
 * solves it, and returns the result as a JSON object.
 *
 * ### Expected Input
 * - `variables`: Array of variable definitions, each with `name`, `type`, and a `domain`
 *   (either `interval = [lb, ub]` or `values = [...]`).
 * - `constraints`: Optional array of constraint definitions, supporting (more to come):
 *   - `all_different`
 *   - `linear`
 *   - `equal`, `not_equal`, `greater_than`, `less_than`
 *   - `forbidden_assignments` (currently simplified to force zero for each listed variable)
 *
 * ### Return Format
 * - On success:
 *   ```
 *   {
 *     "status": "solved",
 *     "assignments": {
 *       "varA": 1,
 *       "varB": 2
 *     }
 *   }
 *   ```
 * - On failure (e.g., missing `variables`):
 *   ```
 *   {
 *     "status": "error",
 *     "message": "No variables specified in problem."
 *   }
 *   ```
 * - No solution response:
 *   ```
 *   {
 *     "status": "no_solution"
 *   }
 *   ```
 *
 * @param problem A JSON object representing a validated CSP problem description.
 * @return A JSON object describing the solution, failure, or error state.
 *
 * @ingroup internal
 * @see map_problem()
 */

json CSPScheduler::solve(const json& problem) {
    CpModelBuilder model_builder;
    unordered_map<string, IntVar> var_map;
    vector<string> var_names;
    vector<BoolVar> soft_constraint_ind;
    vector<double> soft_constraint_weights;
    json result_json;
    Model cp_model;

    //safety checks if validator fails, should always pass through before 
    if (!problem.contains("variables")) {
        result_json["status"] = "error";
        result_json["message"] = "No variables specified in problem.";
        return result_json;
    }

    map_problem(problem, model_builder, var_names, var_map, soft_constraint_ind, soft_constraint_weights);

    if (!soft_constraint_ind.empty()) {
        LinearExpr soft_violation_sum;
        for (size_t i = 0; i < soft_constraint_ind.size(); ++i) {
            soft_violation_sum += soft_constraint_weights[i] * soft_constraint_ind[i];
        }
        //always minimize # of violations, not minimize objective of schema
        model_builder.Minimize(soft_violation_sum);
    }

    const CpSolverResponse response = SolveCpModel(model_builder.Build(), &cp_model);

    if (response.status() == CpSolverStatus::FEASIBLE || response.status() == CpSolverStatus::OPTIMAL) {
        result_json["status"] = "solved";
        json assignments = json::object();
        for (const auto& name : var_names) {
            assignments[name] = SolutionIntegerValue(response, var_map[name]);
        }
        result_json["assignments"] = assignments;
    } else {
        result_json["status"] = "no_solution";
    }
    return result_json;
}

//TODO: This should probably pull the parts directly from json_validator/ schema. Another struct that is created from the parse
// json validated schema could make the map_problem a lot less hard-coded?
/**
 * Convert a high-level JSON CSP problem description into an OR-Tools model.
 *
 * This helper parses JSON-defined variables and constraints and
 * translates them into CP-SAT model primitives using the OR-Tools
 * C++ interface (`CpModelBuilder`, `IntVar`, and constraints).
 *
 * Variable types supported:
 * - `"int"` and `"bool"` only, with domain as `[lb, ub]` or `values: [...]`
 *
 * Constraint types supported:
 * - `"all_different"`
 * - `"linear"` — with optional coefficients, relation (e.g., `"equal"`, `"greater_than"`), and target value
 * - `"equal"`, `"not_equal"`, `"greater_than"`, `"less_than"` — for one var and constant
 * - `"forbidden_assignments"` — naive implementation: all involved vars are constrained to 0
 *
 * @param problem The parsed CSP input in structured JSON format.
 * @param model_builder Output CP-SAT builder to which constraints/vars are added.
 * @param var_names Output list of variable names (to track assignment order).
 * @param var_map Output mapping from variable names to their IntVar representations.
 * @param soft_constraint_ind Soft constraint indicators, values that id when violation occurs.
 * @param soft_constraint_weights Soft constraint weights, values that scale the violations.
 *
 * @ingroup internal
 */
void CSPScheduler::map_problem(
    const json& problem, 
    CpModelBuilder& model_builder,
    vector<string>& var_names,
    unordered_map<string, IntVar>& var_map,
    vector<BoolVar>& soft_constraint_ind,
    vector<double>& soft_constraint_weights) 
{
    for (const auto& var : problem["variables"]) {
        if (var["type"] == "int" || var["type"] == "bool") {
            int lb = 0, ub = 0;
            if (var["domain"].contains("interval")) {
                lb = var["domain"]["interval"][0];
                ub = var["domain"]["interval"][1];
            } else if (var["domain"].contains("values")) {
                auto vals = var["domain"]["values"];
                lb = *std::min_element(vals.begin(), vals.end());
                ub = *std::max_element(vals.begin(), vals.end());
            }
            string name = var["name"];
            IntVar int_var = model_builder.NewIntVar(Domain(lb, ub)).WithName(name);
            var_names.push_back(name);
            var_map[name] = int_var;
        }
    }

    if (problem.contains("constraints")) {
        for (const auto& constraint : problem["constraints"]) {
            string type = constraint["type"];
            vector<string> involved_vars = constraint["variables"];

            bool loosely_constrained = constraint.contains("loose");
            BoolVar violation_ind;

            if (loosely_constrained) {
                violation_ind = model_builder.NewBoolVar();
                double weight = constraint["loose"].value("weight", 1.0);
                soft_constraint_ind.push_back(violation_ind);
                soft_constraint_weights.push_back(weight);
            }

            if (type == "all_different") {
                vector<IntVar> vars;
                for (const auto& vname : involved_vars) {
                    vars.push_back(var_map.at(vname));
                }
                if (!loosely_constrained) {
                    model_builder.AddAllDifferent(vars);
                } else {
                    //TODO: loose constraint handling tbd
                    model_builder.AddAllDifferent(vars);
                }
                model_builder.AddAllDifferent(vars);
            }
            else if (type == "linear") {
                vector<IntVar> vars;
                for (const auto& vname : involved_vars) {
                    vars.push_back(var_map.at(vname));
                }
                vector<int64_t> coeffs = constraint.contains("coefficients") ? constraint["coefficients"].get<vector<int64_t>>() : vector<int64_t>(vars.size(), 1);
                string rel = constraint.value("relation", "equal");
                int64_t rhs = constraint.value("value", 0);

                LinearExpr expr;
                for (size_t i = 0; i < vars.size(); ++i) {
                    expr += coeffs[i] * vars[i];
                }
                if (!loosely_constrained) {
                    if (rel == "equal") {
                        model_builder.AddEquality(expr, rhs);
                    } else if (rel == "greater_than") {
                        model_builder.AddGreaterOrEqual(expr, rhs);
                    } else if (rel == "less_than") {
                        model_builder.AddLessOrEqual(expr, rhs);
                    } else {
                        std::cerr << "Unknown linear relation: " << rel << std::endl;
                    }
                } else {
                    //TODO: loose constraint handling tbd
                    if (rel == "equal") {
                        model_builder.AddEquality(expr, rhs);
                    } else if (rel == "greater_than") {
                        model_builder.AddGreaterOrEqual(expr, rhs);
                    } else if (rel == "less_than") {
                        model_builder.AddLessOrEqual(expr, rhs);
                    } else {
                        std::cerr << "Unknown linear relation: " << rel << std::endl;
                    }
                }
            }
            else if (type == "equal" || type == "not_equal" || type == "greater_than" || type == "less_than") {
                if (involved_vars.size() == 1 && constraint.contains("value")) {
                    IntVar var = var_map.at(involved_vars[0]);
                    int64_t val = constraint["value"];
                    if (!loosely_constrained) {
                        if (type == "equal") {
                            model_builder.AddEquality(var, val);
                        } else if (type == "not_equal") {
                            model_builder.AddNotEqual(var, val);
                        } else if (type == "greater_than") {
                            model_builder.AddGreaterOrEqual(var, val);
                        } else if (type == "less_than") {
                            model_builder.AddLessOrEqual(var, val);
                        }
                    } else {
                        //TODO: loose constraint handling tbd
                        if (type == "equal") {
                            model_builder.AddEquality(var, val);
                        } else if (type == "not_equal") {
                            model_builder.AddNotEqual(var, val);
                        } else if (type == "greater_than") {
                            model_builder.AddGreaterOrEqual(var, val);
                        } else if (type == "less_than") {
                            model_builder.AddLessOrEqual(var, val);
                        }
                    }
                }
            }
            else if (type == "forbidden_assignments") {
                for (const auto& vname : involved_vars) {
                    model_builder.AddEquality(var_map.at(vname), 0);
                }
            }
            else if (type == "allowed_assignments") {
                vector<IntVar> vars;
                for (const auto& vname : involved_vars) {
                    vars.push_back(var_map.at(vname));
                }
                vector<vector<int64_t>> allowed_values = constraint["allowed_assignments"].get<vector<vector<int64_t>>>();

                TableConstraint table = model_builder.AddAllowedAssignments(vars);
                for (const auto& tuple : allowed_values) {
                    table.AddTuple(tuple);
                }
            }
            else if (type == "implication") {
                BoolVar A_is_true = model_builder.NewBoolVar();
                IntVar B_var = var_map.at(involved_vars[1]);
                int64_t B_val = constraint["value"];
                model_builder.AddEquality(B_var, B_val).OnlyEnforceIf(A_is_true);
            }
            else if(type == "element") {
                IntVar index_var = var_map.at(involved_vars[0]);
                IntVar target_var = var_map.at(involved_vars[1]);

                vector<int64_t> values = constraint["values"].get<vector<int64_t>>();

                vector<IntVar> var_arr;
                //this is wrong and needs to map things around, look at CP-SAT doc
                model_builder.AddElement(index_var, values, target_var);
            }
        }
    }
}
