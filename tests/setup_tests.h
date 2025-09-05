#ifndef SETUP_TESTS_H
#define SETUP_TESTS_H

#include "CivetServer.h"
#include <gtest/gtest.h>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <string>
#include <map>
#include <vector>

#include "../src/server.h"
#include "../src/routes.h"
#include "../src/routes/RootHandler.h"
#include "../src/routes/CSPSolverHandler.h"

using namespace std;

struct HttpResponse {
    long status_code{};
    string body;
    map<string, string> headers;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t total_size = size * nitems;
    string header(buffer, total_size);
    auto* headers = static_cast<map<string, string>*>(userdata);

    auto pos = header.find(':');
    if (pos != string::npos) {
        string key = header.substr(0, pos);
        string value = header.substr(pos + 1);
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        (*headers)[key] = value;
    }
    return total_size;
}

inline HttpResponse httpRequest(const string& url,
                         const string& method = "GET",
                         const string& data = "") {
    HttpResponse response;
    CURL* curl = curl_easy_init();
    if (!curl) return response;

    string response_body;
    map<string, string> response_headers;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    struct curl_slist* headers = nullptr;

    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

        if (!data.empty()) {
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
    } else if (method == "OPTIONS") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    } else if (method != "GET") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
        response.body = response_body;
        response.headers = response_headers;
    } else {
        response.status_code = -1;
        response.body = curl_easy_strerror(res);
    }

    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);

    return response;
}

class CivetWebTest : public ::testing::Test {
protected: 
    CivetServer* server = nullptr;

    RootHandler rootHandler;
    CSPSolverHandler solveHandler;

    void SetUp() override {
        routes.clear();
        routes = {"/", "/solve"}; // Need to do this to satisfy test global mutable

        server = startServer(routes);

        map<string, CivetHandler*> route_map = {
            {"/", &rootHandler},
            {"/solve", &solveHandler}
        };

        for (const auto& [route, handler] : route_map) {
            addRoute(server, route, *handler);
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    void TearDown() override {
        if (server) {
            shutdownServer(server);
            server = nullptr;
        }
    }
};

#endif
