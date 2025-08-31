#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include "CSPSolverHandler.h"
#include "CivetServer.h"
#include "../util/SchemaValidator.h"
#include "../engine/CSPScheduler.h"

using namespace std;
using namespace nlohmann;

/**
 * Handles HTTP POST requests to the CSP solver endpoint.
 *
 * This function receives an HTTP POST request with a JSON-formatted payload
 * that defines a Constraint Satisfaction Problem (CSP) following the
 * `schedule_contract.schema.json` specification.
 *
 * The request is:
 * - **Validated** against the schema.
 * - **Solved** using an internal CSP solver.
 *
 * ### Request Format
 * Expected JSON must conform to the schema described in `schedule_contract.schema.json`.
 *
 * ### Responses
 * - **200 OK**: On successful validation and solving. The response contains a JSON object
 *   with the computed schedule or solution.
 * - **400 Bad Request**: If the payload is missing, malformed, or fails schema validation,
 *   a plain-text error message is returned.
 *
 * @param server Pointer to the CivetServer instance managing HTTP I/O. *(Internal use only.)*
 * @param conn Connection handle representing the client request/response. *(Internal use only.)*
 * @return Always returns true to indicate the request has been fully handled by the server. *(Not meaningful for API callers.)*
 *
 * @ingroup api
 * @see schedule_contract.schema.md
 */
bool CSPSolverHandler::handlePost(CivetServer *server, struct mg_connection *conn) {
    CSPScheduler scheduler;
    string error;

    json schema_json;
    ifstream("../schema/schedule_contract.schema.json") >> schema_json;

    const struct mg_request_info *req_info = mg_get_request_info(conn);
    long long content_length = req_info->content_length;
    if (content_length <= 0) {
        mg_printf(conn, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nNo payload\n");
        return true;
    }
    string payload(content_length, '\0');
    mg_read(conn, &payload[0], content_length);

    json message_json;
    try {
        message_json = json::parse(payload);
    } catch (const exception& e) {
        mg_printf(conn, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON: %s\n", e.what());
        return true;
    }

    bool valid = isJsonValidatedToSchema(schema_json, message_json, &error);
    if (!valid) {
        mg_printf(conn, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nValidation failed: %s\n", error.c_str());
        return true;
    }

    json result_json = scheduler.solve(message_json);

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "%s\n", result_json.dump().c_str());
    return true;
}


