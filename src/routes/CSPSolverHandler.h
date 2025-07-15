#pragma once
#include "CivetServer.h"

/**
 * @brief Class of handlers for /solve endpoints
 */
class CSPSolverHandler : public CivetHandler {
public:
    /**
    * @ingroup api
    * @brief handle the http POST of csp problem json schema
    */
    bool handlePost(CivetServer *server, struct mg_connection *conn) override;
};