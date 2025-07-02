#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <vector>
#include "config/ConfigParser.hpp"

void print_configs(const std::vector<Config>& configs) {
    if (configs.empty()) {
        std::cout << "  No configs found\n";
        return;
    }

    for (const auto& config : configs) {
        std::cout << "  --- Parsed Config for: " << config.name << " ---\n";
        std::cout << "    Category: " << config.category << "\n";
        std::cout << "    Filename: " << config.filename << "\n";
        std::cout << "    runMode: " << config.get_run_mode() << "\n";
        std::cout << std::format("    Args: {}\n", config.additional_args.value_or("(not set)"));
        std::cout << std::format("    Description: {}\n\n", config.description.value_or("(not set)"));
    }
}

int main() {
    const std::filesystem::path examples_dir = "../examples";
    std::vector<std::filesystem::path> test_files;

    if (std::filesystem::exists(examples_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(examples_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                test_files.push_back(entry.path());
            }
        }
    }

    std::ranges::sort(test_files);

    if (test_files.empty()) {
        std::cout << "No JSON files found in " << examples_dir << "\n";
        return 1;
    }

    for (const auto& file : test_files) {
        try {
            std::cout << "\n=== Processing: " << file.filename() << " ===\n";
            ConfigParser parser(file);

            std::cout << "\nAll configs:\n";
            auto all_configs = parser.get_configs();
            print_configs(all_configs);

            std::cout << "\nWinDeploy configs (mode 4):\n";
            auto win_deploy_configs = parser.take_configs(RunMode::WinDeploy);
            print_configs(win_deploy_configs);

        } catch (const std::exception& e) {
            std::cerr << "Error processing " << file << ": " << e.what() << "\n";
        }
    }

    return 0;
}
