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
#include "../src/routes/RootHandler.h"
#include "../src/routes/CSPSolverHandler.h"

using namespace std;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class CivetWebTest : public ::testing::Test {
protected:
    CivetServer* server = nullptr;

    RootHandler rootHandler;
    CSPSolverHandler solveHandler;

    void SetUp() override {
        vector<string> routes;
        server = startServer(routes);

        map<string, CivetHandler*> route_map = {
            {"/", &rootHandler},
            {"/solve", &solveHandler}
        };

        for (const auto& [route, handler] : route_map) {
            routes.push_back(route);
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

    string httpRequest(const string& url, const string& method = "GET") {
        CURL* curl = curl_easy_init();
        string response;

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            if (method == "POST") {
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
            }
            CURLcode res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            curl_easy_cleanup(curl);
        }
        return response;
    }
};

#endif
