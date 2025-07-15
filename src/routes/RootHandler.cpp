#include "RootHandler.h"
#include "../routes.h"

/**
 * Handles the HTTP GET request for the root ("/") endpoint.
 *
 * Responds with a plain text welcome message and basic guidance
 * for using the CP-SAT Solver API.
 *
 * ### Response
 * - **200 OK**
 *   - Content-Type: text/plain
 *   - Body: Message introducing available endpoints (e.g., "Try POST /solve")
 *
 * Intended as a human-readable landing endpoint.
 *
 * @param server Unused server pointer (internal).
 * @param conn Connection object representing the client's HTTP request.
 * @return Always returns true to signal the request was handled.
 *
 * @ingroup api
 */
bool RootHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "Welcome to the CP-SAT Solver API!\nTry POST /solve or OPTIONS /");
    return true;
}

/**
 * Handles the HTTP OPTIONS request for the root ("/") endpoint.
 *
 * Returns a JSON list of available public API endpoints and allowed HTTP methods.
 * This is useful for tools or clients performing CORS pre-flight checks
 * or needing dynamic discovery of endpoint capabilities.
 *
 * ### Response
 * - **200 OK**
 *   - Content-Type: application/json
 *   - Headers:
 *     - Allow: GET, POST, OPTIONS
 *   - Body:
 *     ```
 *     {
 *       "routes": ["/", "/solve", "/status"]
 *     }
 *     ```
 *
 * @param server Unused server pointer (internal).
 * @param conn Connection object representing the client's HTTP request.
 * @return Always returns true to signal the request was handled.
 *
 * @ingroup api
 */

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
