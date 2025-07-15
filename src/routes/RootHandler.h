#pragma once
#include "CivetServer.h"

/**
 * @defgroup api Public API
 * @brief Public-facing HTTP endpoint handlers.
 *
 * This group includes all top-level HTTP API classes and functions
 * that expose the CSP solver to client integrations.
 */

/**
 * @brief Handles root ("/") endpoint requests for welcome and discovery.
 */
class RootHandler : public CivetHandler {
public:
    /**
    * @ingroup api
    * @brief handle the http GET of / and return dummy message
    */
    bool handleGet(CivetServer*, struct mg_connection*) override;
    /**
    * @ingroup api
    * @brief handle the http OPTIONS and return public API endpoints
    */
    bool handleOptions(CivetServer*, struct mg_connection*) override;
};