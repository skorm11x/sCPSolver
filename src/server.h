#pragma once
#include "CivetServer.h"
#include <vector>
#include <string>

CivetServer* startServer(std::vector<std::string>& routes);
int shutdownServer(CivetServer* server);
void addRoute(CivetServer* server, const std::string& route, CivetHandler& handler);
