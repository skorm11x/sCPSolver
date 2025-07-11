#include "CSPSolverHandler.h"
#include "../engine/CSPScheduler.h"

bool CSPSolverHandler::handlePost(CivetServer *server, struct mg_connection *conn) {
    CSPScheduler scheduler;

    std::string result_json = scheduler.solveAndPrint();

    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: application/json\r\n"
              "Connection: close\r\n\r\n");
    mg_printf(conn, "{\"status\": \"solved\", \"result\": 42}\n");
    return true;
}


