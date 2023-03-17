#include <iostream>

#include "cxxopts/cxxopts.hpp"

#include "cli/options.h"

int main(int argc, char** argv) {
    // Define an argument parser.
    cxxopts::Options cli_options(
            "rankd", "Monitoring interface for remote nodes to get data on current status and capabilities.");
    define_cli_options(cli_options);
    auto cli = cli_options.parse(argc, argv);

    if (cli.count("help")) {
        std::cout << cli_options.help() << std::endl;
        std::exit(0);
    }

    std::cout << "Hello World!" << std::endl;

    return 0;
}