#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <iostream>
#include <fstream>
#include <string>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

/**
 * Implementation of JSON Schema validation using nlohmann::json-schema.
 *
 * Constructs a validator from the provided schema and attempts to
 * validate the given message. If validation fails and an errorMsg
 * pointer is provided, it stores the exception message for reference.
 */
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
