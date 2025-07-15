#include <unistd.h>
#include <map>
#include <string>
#include <iostream>
#include "CivetServer.h"

#include "routes.h"

#include "routes/RootHandler.h"
#include "routes/CSPSolverHandler.h"

using namespace std;

/**
 * @defgroup internal Internal workings
 * @brief starts the Civetweb server and loads routes
 * 
 */
int main() {
    const char *options[] = { "listening_ports", "8080", 0 };
    CivetServer server(options);

    RootHandler rootHandler;
    CSPSolverHandler solveHandler;

    map<string, CivetHandler*> route_map = {
        {"/", &rootHandler},
        {"/solve", &solveHandler}
    };

    for (const auto& [route, handler] : route_map) {
        routes.push_back(route);
        server.addHandler(route, *handler);
    }

    cout << "started scpsolver at localhost:8080" << endl;

    while (true) { sleep(1); }
    return 0;
}
