#include "setup_tests.h"
#include "gtest/gtest.h"
#include <curl/curl.h>
#include <string>

using namespace std;

TEST_F(CivetWebTest, RootRouteReturnsWelcome) {
    auto resp = httpRequest("http://localhost:8080/");
    EXPECT_NE(resp.find("Welcome"), string::npos);
}