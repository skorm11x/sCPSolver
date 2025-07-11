#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include "CSPSolverHandler.h"
#include "CivetServer.h"
#include "../util/SchemaValidator.h"
#include "../engine/CSPScheduler.h"

bool CSPSolverHandler::handlePost(CivetServer *server, struct mg_connection *conn) {
    CSPScheduler scheduler;
    std::string error;

    nlohmann::json schema_json;
    std::ifstream("../schema/schedule_contract.schema.json") >> schema_json;

    const struct mg_request_info *req_info = mg_get_request_info(conn);
    long long content_length = req_info->content_length;
    if (content_length <= 0) {
        mg_printf(conn, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nNo payload\n");
        return true;
    }
    std::string payload(content_length, '\0');
    mg_read(conn, &payload[0], content_length);

    nlohmann::json message_json;
    try {
        message_json = nlohmann::json::parse(payload);
    } catch (const std::exception& e) {
        mg_printf(conn, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON: %s\n", e.what());
        return true;
    }

    bool valid = isJsonValidatedToSchema(schema_json, message_json, &error);
    if (!valid) {
        mg_printf(conn, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nValidation failed: %s\n", error.c_str());
        return true;
    }

    nlohmann::json result_json = scheduler.solve(message_json);

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "%s\n", result_json.dump().c_str());
    return true;
}


