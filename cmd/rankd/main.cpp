#include <iostream>

#include "cxxopts/cxxopts.hpp"
#include "restbed"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "internals.h"
#include "cli/options.h"
#include "configs/configs.h"

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

    // Set a logger.
    auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_logger->set_pattern("%E.%e [%^%l%$] %v");  // Format is "UNIXTS [level] <message>".
    auto file_logger =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::string(getenv("HOME")) + "/.rank/execution.log");
    file_logger->set_pattern("%E.%e [%^%l%$] %v");  // Format is "UNIXTS [level] <message>".

    bool debug = cli.count("debug") > 0;
    std::vector<spdlog::sink_ptr> single_sink({console_logger});
    std::vector<spdlog::sink_ptr> multiple_sinks({console_logger, file_logger});
    auto logger = debug ? std::make_shared<spdlog::logger>("rankd-logger", begin(multiple_sinks), end(multiple_sinks))
                        : std::make_shared<spdlog::logger>("rankd-logger", begin(single_sink), end(single_sink));
    spdlog::register_logger(logger);

    size_t logger_depth = cli.count("verbose");
    if (logger_depth == 2) {
        logger->set_level(spdlog::level::debug);
        logger->flush_on(spdlog::level::debug);
    } else if (logger_depth > 2) {
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);
    } else {
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);
    }

    // Read configuration file, if specified.
    std::string configuration_file_path;
    if (cli.count("config")) {
        configuration_file_path = cli["config"].as<std::string>();
    } /*else {
        logger->critical("Failed to configure the execution, since no configuration file was provided.");
        std::cerr << "A configuration path is needed. Please use the option -c(--config) <configuration-file-path>."
                  << std::endl;
        std::cout << cli_options.help() << std::endl;

        // Quit program with Invalid Arguments (22) error.
        exit(EINVAL);
    }*
    Configuration* configuration = parse_configuration(configuration_file_path);
    if (configuration == nullptr) {
        logger->critical("A badly-written configuration file was given.");
        std::cerr << "A badly-written configuration file was given. Please fix it and try again." << std::endl;

        // Quit program with Invalid Arguments (22) error.
        exit(EINVAL);
    }*/

    logger->info("The rankd service is starting.");

    std::vector<std::shared_ptr<restbed::Resource>> api_resources;

    auto* computing_engine = ComputingEngine::get_instance();
    auto* network_engine = NetworkEngine::get_instance();
    auto* time_engine = TimeEngine::get_instance();
    api_resources.push_back(computing_engine->execute());
    api_resources.push_back(network_engine->execute());
    api_resources.push_back(time_engine->execute());

    std::shared_ptr<restbed::Settings> api_settings = std::make_shared<restbed::Settings>();
    api_settings->set_port(7265);
    api_settings->set_default_header("Connection", "close");
    api_settings->set_worker_limit(std::thread::hardware_concurrency());

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    computing_engine->stop();
    network_engine->stop();
    time_engine->stop();

    return 0;
}