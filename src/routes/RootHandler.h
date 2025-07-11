#pragma once
#include "CivetServer.h"
#include <vector>
#include <string>

class RootHandler : public CivetHandler {
public:
    bool handleGet(CivetServer*, struct mg_connection*) override;
    bool handleOptions(CivetServer*, struct mg_connection*) override;
};