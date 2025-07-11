#pragma once
#include <nlohmann/json.hpp>

class CSPScheduler {
public:
    nlohmann::json solve(const nlohmann::json& problem);
};