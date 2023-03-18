#ifndef RANKD_CONFIGS_H
#define RANKD_CONFIGS_H

#include <fstream>
#include <optional>

#include "toml++/toml.hpp"

struct Configuration {
    std::optional<std::string> title;
};

Configuration* parse_configuration(const std::string& configuration_path);

#endif  // RANKD_CONFIGS_H
