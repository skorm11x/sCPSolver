#include "setup_tests.h"
#include "gtest/gtest.h"
#include <curl/curl.h>
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

using namespace std;

TEST_F(CivetWebTest, RootRouteReturnsWelcome) {
    auto resp = ::httpRequest("http://localhost:8080/");
    EXPECT_NE(resp.body.find("Welcome"), std::string::npos);
}

TEST_F(CivetWebTest, RootRouteProvidesOptions) {
    auto resp = ::httpRequest("http://localhost:8080/", "OPTIONS");

    EXPECT_EQ(resp.status_code, 200);

    auto contentTypeIter = resp.headers.find("Content-Type");
    ASSERT_NE(contentTypeIter, resp.headers.end());
    EXPECT_EQ(contentTypeIter->second, "application/json");

    auto allowIter = resp.headers.find("Allow");
    ASSERT_NE(allowIter, resp.headers.end());
    EXPECT_NE(allowIter->second.find("GET"), string::npos);
    EXPECT_NE(allowIter->second.find("POST"), string::npos);
    EXPECT_NE(allowIter->second.find("OPTIONS"), string::npos);

    auto jsonBody = nlohmann::json::parse(resp.body);
    ASSERT_TRUE(jsonBody.contains("routes"));
    auto routes = jsonBody["routes"];
    EXPECT_TRUE(routes.is_array());

    cout << "routes are: " << routes << "for jsonBody: " << jsonBody << endl;

    vector<string> expectedRoutes = {"/", "/solve"};
    for (const auto& route : expectedRoutes) {
        EXPECT_NE(find(routes.begin(), routes.end(), route), routes.end());
    }
}

TEST_F(CivetWebTest, SolverRouteDummyEmptyPayloadFails) {
    string dummy_post_data = "";
    auto resp = ::httpRequest("http://localhost:8080/solve", "POST", dummy_post_data);

    EXPECT_EQ(resp.status_code, 400);

    auto contentTypeIter = resp.headers.find("Content-Type");
    ASSERT_NE(contentTypeIter, resp.headers.end());
    EXPECT_EQ(contentTypeIter->second, "text/plain");

    EXPECT_EQ(resp.body, "No payload\n");
}

TEST_F(CivetWebTest, SolverRouteDummyInvalidJsonFails) {
    string invalid_json_data = "{ bad boi json is bad json }";

    auto resp = ::httpRequest("http://localhost:8080/solve", "POST", invalid_json_data);

    EXPECT_EQ(resp.status_code, 400);

    auto contentTypeIter = resp.headers.find("Content-Type");
    ASSERT_NE(contentTypeIter, resp.headers.end());
    EXPECT_EQ(contentTypeIter->second, "text/plain");

    string expected_prefix = "Invalid JSON:";
    EXPECT_TRUE(resp.body.compare(0, expected_prefix.size(), expected_prefix) == 0);
}
