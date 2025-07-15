/**
 * @file CSPScheduler.h
 * @author skorm (skorm11x)
 * @brief 
 * @version 0.1
 * @date 2025-07-14
 */

#pragma once
#include <nlohmann/json.hpp>
#include <ortools/sat/cp_model.h>
#include <ortools/sat/cp_model.pb.h>

/**
 * @brief 
 * 
 */
class CSPScheduler {
public:
    nlohmann::json solve(const nlohmann::json& problem);
private:
    void map_problem(
        const nlohmann::json& problem,
        operations_research::sat::CpModelBuilder& model_builder,
        std::vector<std::string>& var_names,
        std::unordered_map<std::string, operations_research::sat::IntVar>& var_map
    );
};