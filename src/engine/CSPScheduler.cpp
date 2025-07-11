#include "CSPScheduler.h"
#include <ortools/sat/cp_model.h>
#include <ortools/sat/cp_model.pb.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace operations_research;
using namespace sat;
using namespace nlohmann;
using namespace std;

json CSPScheduler::solve(const json& problem) {
    CpModelBuilder model;

    vector<IntVar> int_vars;
    vector<string> var_names;
    unordered_map<string, IntVar> var_map;

    json result_json;

    // 1. Variables
    if (!problem.contains("variables")) {
        result_json["status"] = "error";
        result_json["message"] = "No variables specified in problem.";
        return result_json;
    }

    for (const auto& var : problem["variables"]) {
        if (var["type"] == "int") {
            int lb = 0, ub = 0;
            if (var["domain"].contains("interval")) {
                lb = var["domain"]["interval"][0];
                ub = var["domain"]["interval"][1];
            } else if (var["domain"].contains("values")) {
                auto vals = var["domain"]["values"];
                lb = *min_element(vals.begin(), vals.end());
                ub = *max_element(vals.begin(), vals.end());
            }
            string name = var["name"];
            IntVar int_var = model.NewIntVar(Domain(lb, ub)).WithName(name);
            int_vars.push_back(int_var);
            var_names.push_back(name);
            var_map[name] = int_var;
        }
        // Add other types as needed
    }

    // 2. Constraints
    if (problem.contains("constraints")) {
        for (const auto& constraint : problem["constraints"]) {
            string type = constraint["type"];
            vector<string> involved_vars = constraint["variables"];

            if (type == "all_different") {
                vector<IntVar> vars;
                for (const auto& vname : involved_vars) {
                    vars.push_back(var_map[vname]);
                }
                model.AddAllDifferent(vars);
            }
            // Add more constraint types as needed
        }
    }

    // 3. Solve
    Model cp_model;
    const CpSolverResponse response = SolveCpModel(model.Build(), &cp_model);

    // 4. Prepare JSON result
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
