#include "utils/identity.h"

#include <array>
#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

using namespace std;

int main(int argc, char** argv) {
    doctest::Context context;

    // !!! THIS IS JUST AN EXAMPLE SHOWING HOW DEFAULTS/OVERRIDES ARE SET !!!

    // defaults
    context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in their name
    context.setOption("abort-after", 5);              // stop test execution after 5 failed assertions
    context.setOption("order-by", "name");            // sort the test cases by their name

    context.applyCommandLine(argc, argv);

    // overrides
    context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests

    int client_stuff_return_code = 0;
    // your program - if the testing framework is integrated in your production code

    return res + client_stuff_return_code; // the result from doctest is propagated here as well
}

TEST_SUITE("rank-prelude-utils-identity") {
    TEST_CASE("rank-prelude-utils-identity-ip4") {
        auto ipv4_example1 = std::array<uint8_t, 4>({127, 0, 0, 1});
        auto ipv4_example2 = std::array<uint8_t, 4>({123, 123, 123, 123});
        auto ipv4_example3 = std::array<uint8_t, 4>({100, 80, 145, 21});
        auto ipv4_example4 = std::array<uint8_t, 4>({10, 0, 23, 156});
        auto ipv4_example5 = std::array<uint8_t, 4>({10, 0, 23, 155});

        CHECK(is_me(ipv4_example1));
        CHECK(not is_me(ipv4_example2));
        CHECK(is_me(ipv4_example3));
        CHECK(is_me(ipv4_example4));
        CHECK(not is_me(ipv4_example5));
    }

    TEST_CASE("rank-prelude-utils-identity-mac") {
        auto mac_example1 = std::array<uint8_t, 6>({0xf0, 0x76, 0x1c, 0xd5, 0x00, 0x78});
        auto mac_example2 = std::array<uint8_t, 6>({0xf0, 0x76, 0x1c, 0xd5, 0x00, 0x79});
        auto mac_example3 = std::array<uint8_t, 6>({02, 0x42, 0x89, 0xc2, 0x99, 0x37});

        CHECK(is_me(mac_example1));
        CHECK(not is_me(mac_example2));
        CHECK(is_me(mac_example3));
    }

    TEST_CASE("rank-prelude-utils-identity-ip6") {
        auto ipv6_example1 = std::array<uint8_t, 16>(
                {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x89, 0xFF, 0xFE, 0xC2, 0x99, 0x37});
        auto ipv6_example2 = std::array<uint8_t, 16>(
                {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x46, 0xA1, 0xE5, 0x10, 0x13, 0x44, 0xF3});
        auto ipv6_example3 = std::array<uint8_t, 16>(
                {0xFE, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x46, 0xA1, 0xE5, 0x10, 0x13, 0x44, 0xF3});
        auto ipv6_example4 = std::array<uint8_t, 16>(
                {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x89, 0xFE, 0xFE, 0xC2, 0x99, 0x37});

        CHECK(is_me(ipv6_example1));
        CHECK(is_me(ipv6_example2));
        CHECK(not is_me(ipv6_example3));
        CHECK(not is_me(ipv6_example4));
    }
}