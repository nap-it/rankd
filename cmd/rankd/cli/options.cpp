#include "cli/options.h"

void define_cli_options(cxxopts::Options& options) {
    options.add_options()
            ("h,help", "Print this help")
            ("v,verbose", "Select verbosity level")
            ("c,config", "Use this configuration file", cxxopts::value<std::string>());
}