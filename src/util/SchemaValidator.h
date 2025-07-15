#pragma once

#include <string>
#include <nlohmann/json_fwd.hpp>

/**
 * @brief Validate a JSON message against a JSON Schema.
 *
 * This utility function uses a JSON schema to check that
 * a given JSON message conforms to the required structure.
 * It is used for pre-processing and validating incoming
 * CSP or API requests before business logic is applied.
 *
 * @param schema The JSON Schema as a `nlohmann::json` object.
 * @param message The JSON object to validate.
 * @param errorMsg Optional output pointer. If validation fails,
 *        the pointed string will be set to the validation error message.
 * @return `true` if the message is valid according to the schema,
 *         `false` otherwise.
 *
 * @ingroup internal
 */
bool isJsonValidatedToSchema(
    const nlohmann::json& schema,
    const nlohmann::json& message, 
    std::string* errorMsg = nullptr
);