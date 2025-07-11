#include "RootHandler.h"
#include "../routes.h"

bool RootHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "Welcome to the CP-SAT Solver API!\nTry POST /solve or OPTIONS /");
    return true;
}

bool RootHandler::handleOptions(CivetServer *server, struct mg_connection *conn) {
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAllow: GET,POST,OPTIONS\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "{\"routes\": [");
    for (size_t i = 0; i < routes.size(); ++i) {
        mg_printf(conn, "\"%s\"%s", routes[i].c_str(), (i < routes.size() - 1) ? "," : "");
    }
    mg_printf(conn, "]}\n");
    return true;
}
