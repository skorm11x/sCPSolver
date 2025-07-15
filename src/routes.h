#pragma once
#include <vector>
#include <string>

/**
 * @brief Globally accessible list of public API routes.
 *
 * Used by the root `/options` endpoint to dynamically report
 * available paths. The contents of this vector define
 * the API interface from a client-facing perspective.
 *
 * Each entry should correspond to a valid HTTP route, e.g., "/solve", "/status".
 *
 * @ingroup api
 */
extern std::vector<std::string> routes;
