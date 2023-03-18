#include "configs/configs.h"

Configuration* parse_configuration(const std::string& configuration_path) {
    // Load configuration as a TOML file in configuration_path.
    toml::parse_result config;
    try {
        config = toml::parse_file(configuration_path);
    } catch (toml::parse_error& pe) {
        return nullptr;
    }
    auto* configuration = new Configuration;

    // Get data on "title".
    if (config.contains("title")) {
        configuration->title = config["title"].value<std::string>();
    }

    return configuration;
}
