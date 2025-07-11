#include "CSPScheduler.h"
#include <ortools/sat/cp_model.h>
#include <ortools/sat/cp_model.pb.h>
#include <iostream>
#include <sstream>

using namespace operations_research;
using namespace sat;

std::string CSPScheduler::solveAndPrint() {
    CpModelBuilder model;

    const int num_jobs = 3; //variables
    std::vector<IntVar> starts;
    for (int i = 0; i < num_jobs; ++i) {
        starts.push_back(model.NewIntVar(Domain(0, 2)).WithName("job" + std::to_string(i+1)));
    }

    model.AddAllDifferent(starts); //constraint is that all must be different/ no overlap
    Model cp_model;
    const CpSolverResponse response = SolveCpModel(model.Build(), &cp_model);

    std::ostringstream out;
    if (response.status() == CpSolverStatus::FEASIBLE || response.status() == CpSolverStatus::OPTIMAL) {
        out << "Schedule:\n";
        for (int i = 0; i < num_jobs; ++i) {
            out << "Job " << (i+1) << " starts at " << SolutionIntegerValue(response, starts[i]) << "\n";
        }
    } else {
        out << "No solution found.\n";
    }
    std::cout << out.str();
    return out.str();
}
