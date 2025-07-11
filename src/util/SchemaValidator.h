#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

bool isJsonValidatedToSchema(
    const nlohmann::json& schema,
    const nlohmann::json& message, 
    std::string* errorMsg = nullptr
);