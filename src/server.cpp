#include "server.h"

CivetServer* startServer(std::vector<std::string>& routes) {
    const char *options[] = { "listening_ports", "8080", 0 };
    CivetServer* server = new CivetServer(options);
    return server;
}

int shutdownServer(CivetServer* server) {
    if (!server) {
        return -1;
    }

    try {
        delete server;  // ~CivetServer() will stop listeners and cleanup
        return 0;
    } catch (...) {
        return -1;
    }
}

void addRoute(CivetServer* server,
    const std::string& route, CivetHandler& handler) {
    server->addHandler(route, handler);
}