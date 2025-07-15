#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <ortools/sat/cp_model.h>
#include <ortools/sat/cp_model.pb.h>

namespace operations_research::sat {
    class CpModelBuilder;
    class IntVar;
}
namespace nlohmann {
    class json;
}

/**
 * @brief Solves constraint satisfaction problems (CSP) using the OR-Tools CP-SAT solver.
 *
 * This class provides an interface for solving CSP problems defined
 * in a structured JSON format. It's designed to take validated problem
 * definitions and produce feasible or optimal assignments using Google's OR-Tools.
 *
 * @ingroup internal
 */
class CSPScheduler {
public:
    /**
     * @brief Solve a constraint satisfaction problem described in JSON form.
     *
     * Accepts a problem object with `variables` and `constraints`, encodes it
     * using the CP-SAT logic, and returns either a result object with assignments or
     * an error message.
     *
     * @param problem A JSON object describing the CSP (validated beforehand).
     * @return A JSON object with solution status, any assignments or errors.
     *
     * @see map_problem()
     */
    json solve(const json& problem);
private:
    /**
     * @brief Translate JSON-defined problem into a CP model (OR-Tools).
     *
     * Converts variable declarations and supported constraints into OR-Tools
     * primitives. This is an internal helper, not intended for external use.
     *
     * @param problem The parsed JSON CSP description.
     * @param model_builder CP-SAT model construct to build on.
     * @param var_names Output list of variable names (order matters for assignment output).
     * @param var_map Output map of variable names to OR-Tools IntVar.
     */
    void map_problem(
        const json& problem,
        CpModelBuilder& model_builder,
        vector<string>& var_names,
        unordered_map<string, IntVar>& var_map
    );
};