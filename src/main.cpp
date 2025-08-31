#include <sys/signal.h>
#include <unistd.h>
#include <map>
#include <string>
#include <csignal>
#include <iostream>
#include "server.h"

#include "routes.h"

#include "routes/RootHandler.h"
#include "routes/CSPSolverHandler.h"

using namespace std;

atomic<bool> server_running(true); //global server state tracking

/**
 * @defgroup internal Internal workings
 * @brief intercept signals for clean server shutdown
 * @param signal 
 */
void signalHandler(int signal) {
    server_running = false;
}

/**
 * @defgroup internal Internal workings
 * @brief starts the Civetweb server and loads routes
 * 
 */
int main() {
    signal(SIGTERM, signalHandler);
    signal(SIGKILL, signalHandler); 

    const char *options[] = { "listening_ports", "8080", 0 };
    CivetServer* server = startServer(routes);

    RootHandler rootHandler;
    CSPSolverHandler solveHandler;

    map<string, CivetHandler*> route_map = {
        {"/", &rootHandler},
        {"/solve", &solveHandler}
    };

    for (const auto& [route, handler] : route_map) {
        routes.push_back(route);
        addRoute(server, route, *handler);
    }

    cout << "started scpsolver at localhost:8080" << endl;

    while (true) { sleep(1); }
    return 0;
}
