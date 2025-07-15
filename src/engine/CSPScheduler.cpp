/**
 * @file CSPScheduler.cpp
 * @author skorm (skorm11x)
 * @brief 
 * @version 0.1
 * @date 2025-07-14
 */

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

/**
 * @brief 
 * 
 * @param problem 
 * @return json 
 */
json CSPScheduler::solve(const json& problem) {
    CpModelBuilder model_builder;
    unordered_map<string, IntVar> var_map;
    vector<string> var_names;
    json result_json;
    Model cp_model;

    //safety checks if validator fails, should always pass through before 
    if (!problem.contains("variables")) {
        result_json["status"] = "error";
        result_json["message"] = "No variables specified in problem.";
        return result_json;
    }

    map_problem(problem, model_builder, var_names, var_map);

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

/**
 * @brief 
 * 
 * @param problem 
 * @param model_builder 
 * @param var_names 
 * @param var_map 
 */
void CSPScheduler::map_problem(
    const json& problem, 
    CpModelBuilder& model_builder,
    vector<string>& var_names,
    unordered_map<string, IntVar>& var_map) 
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

            if (type == "all_different") {
                vector<IntVar> vars;
                for (const auto& vname : involved_vars) {
                    vars.push_back(var_map.at(vname));
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
            else if (type == "equal" || type == "not_equal" || type == "greater_than" || type == "less_than") {
                if (involved_vars.size() == 1 && constraint.contains("value")) {
                    IntVar var = var_map.at(involved_vars[0]);
                    int64_t val = constraint["value"];
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
            else if (type == "forbidden_assignments") {
                for (const auto& vname : involved_vars) {
                    model_builder.AddEquality(var_map.at(vname), 0);
                }
            }
        }
    }
}
