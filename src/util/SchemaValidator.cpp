#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <iostream>
#include <fstream>
#include <string>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

bool isJsonValidatedToSchema(
    const nlohmann::json& schema, 
    const nlohmann::json& message, 
    std::string* errorMsg
) {
    json_validator validator;
    validator.set_root_schema(schema);
    try {
        validator.validate(message);
    } catch (const std::exception& e) {
        if (errorMsg) *errorMsg = e.what();
        return false;
    }
    return true;
}
