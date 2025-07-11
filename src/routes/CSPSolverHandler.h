#pragma once
#include "CivetServer.h"
#include <vector>
#include <string>

class CSPSolverHandler : public CivetHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn) override;
};