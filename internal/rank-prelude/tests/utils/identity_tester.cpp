#include "utils/identity.h"

#include <array>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    auto ipv4_example1 = std::array<uint8_t, 4>({127, 0, 0, 1});
    auto ipv4_example2 = std::array<uint8_t, 4>({123, 123, 123, 123});
    auto ipv4_example3 = std::array<uint8_t, 4>({100, 80, 145, 21});
    auto ipv4_example4 = std::array<uint8_t, 4>({10, 0, 23, 156});
    auto ipv4_example5 = std::array<uint8_t, 4>({10, 0, 23, 155});

    cout << "Testing the is_me function with an IPv4 argument..." << endl;
    cout << "is_me(127.0.0.1) must give true, and gives out: " << boolalpha << is_me(ipv4_example1) << endl;
    cout << "is_me(123.123.123.123) must give false, and gives out: " << boolalpha << is_me(ipv4_example2) << endl;
    cout << "is_me(100.80.145.21) must give true (in hollywood, under Tailscale), and gives out: " << boolalpha << is_me(ipv4_example3) << endl;
    cout << "is_me(10.0.23.156) must give true (in hollywood, under IT network), and gives out: " << boolalpha << is_me(ipv4_example4) << endl;
    cout << "is_me(10.0.23.155) must give false (in hollywood, under IT network), and gives out: " << boolalpha << is_me(ipv4_example5) << endl;

    cout << endl;

    auto mac_example1 = std::array<uint8_t, 6>({0xf0, 0x76, 0x1c, 0xd5, 0x00, 0x78});
    auto mac_example2 = std::array<uint8_t, 6>({0xf0, 0x76, 0x1c, 0xd5, 0x00, 0x79});
    auto mac_example3 = std::array<uint8_t, 6>({02, 0x42, 0x89, 0xc2, 0x99, 0x37});

    cout << "Testing the is_me function with a MAC argument..." << endl;
    cout << "is_me(f0:76:1c:d5:00:78) must give true (in hollywood), and gives out: " << boolalpha << is_me(mac_example1) << endl;
    cout << "is_me(f0:76:1c:d5:00:79) must give false (in hollywood), and gives out: " << boolalpha << is_me(mac_example2) << endl;
    cout << "is_me(02:42:89:c2:99:37) must give true (in hollywood), and gives out: " << boolalpha << is_me(mac_example3) << endl;

    cout << endl;

    auto ipv6_example1 = std::array<uint8_t, 16>({0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x89, 0xFF, 0xFE, 0xC2, 0x99, 0x37});
    auto ipv6_example2 = std::array<uint8_t, 16>({0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x46, 0xA1, 0xE5, 0x10, 0x13, 0x44, 0xF3});
    auto ipv6_example3 = std::array<uint8_t, 16>({0xFE, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x46, 0xA1, 0xE5, 0x10, 0x13, 0x44, 0xF3});
    auto ipv6_example4 = std::array<uint8_t, 16>({0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x89, 0xFE, 0xFE, 0xC2, 0x99, 0x37});

    cout << "Testing the is_me function with an IPv6 argument..." << endl;
    cout << "is_me(...) must give true (in hollywood), and gives out: " << boolalpha << is_me(ipv6_example1) << endl;
    cout << "is_me(...) must give true (in hollywood), and gives out: " << boolalpha << is_me(ipv6_example2) << endl;
    cout << "is_me(...) must give false (in hollywood), and gives out: " << boolalpha << is_me(ipv6_example3) << endl;
    cout << "is_me(...) must give false (in hollywood), and gives out: " << boolalpha << is_me(ipv6_example4) << endl;
}