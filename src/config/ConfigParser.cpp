#include "ConfigParser.hpp"
#include <charconv>
#include <filesystem>
#include <fstream>
#include <format>
#include <stdexcept>

ConfigParser::ConfigParser(const std::filesystem::path& config_file) {
    if (std::error_code ec; !std::filesystem::exists(config_file, ec) || ec) {
        throw std::runtime_error(
            std::format("File does not exist or cannot be accessed: {}", config_file.string()));
    }

    std::ifstream file_stream(config_file);
    if (!file_stream.is_open()) {
        throw std::runtime_error(std::format("Failed to open file for reading: {}", config_file.string()));
    }

    m_jsonData = nlohmann::json::parse(file_stream, nullptr, false);
    if (m_jsonData.is_discarded()) {
        throw std::runtime_error(std::format("Invalid JSON format in file: {}", config_file.string()));
    }
}

ConfigParser::ConfigParser(const std::string& json_value) {
    m_jsonData = nlohmann::json::parse(json_value, nullptr, false);
    if (m_jsonData.is_discarded()) {
        throw std::runtime_error("Invalid JSON format in the input string.");
    }
}

std::vector<Config> ConfigParser::get_configs() const {
    if (!m_jsonData.is_array()) {
        throw std::runtime_error("JSON root must be an array of configs.");
    }

    std::vector<Config> configs;
    configs.reserve(m_jsonData.size());
    for (const auto& item : m_jsonData) {
        configs.push_back(parse_single_config(item));
    }
    return configs;
}

std::vector<Config> ConfigParser::take_configs(RunMode mode) const {
    auto all_configs = get_configs();
    std::vector<Config> filtered;
    for (auto config : all_configs) {
        if (config.runMode == mode) {
            filtered.push_back(config);
        }
    }
    return filtered;
}

Config ConfigParser::parse_single_config(const nlohmann::json& json) {
    auto is_valid = [](int val) {
        switch (static_cast<RunMode>(val)) {
        case RunMode::WinDeploy:
        case RunMode::SetupComplete:
        case RunMode::UserFirstLogonUser:
        case RunMode::UserFirstLogonAdmin:
        case RunMode::UserFirstLogonTrustedInstaller:
            return true;
        default:
            return false;
        }
    };

    Config config;
    config.category = json.at("category").get<std::string>();
    config.name = json.at("name").get<std::string>();
    config.filename = json.at("filename").get<std::string>();

    if (const auto it = json.find("runMode"); it != json.end() && !it->is_null()) {
        const auto& runWhenNode = *it;
        int parsed_value = -1;
        auto parse_success = false;

        if (runWhenNode.is_number()) {
            parsed_value = runWhenNode.get<int>();
            parse_success = true;
        } else if (runWhenNode.is_string()) {
            const auto& s = runWhenNode.get_ref<const std::string&>();
            if (const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), parsed_value);
                ec == std::errc() && ptr == s.data() + s.size()) {
                parse_success = true;
            }
        }

        if (parse_success && is_valid(parsed_value)) {
            config.runMode = static_cast<RunMode>(parsed_value);
        }
    }

    if (json.contains("additional_args")) {
        config.additional_args = json.at("additional_args").get<std::string>();
    }

    if (json.contains("description")) {
        config.description = json.at("description").get<std::string>();
    }

    return config;
}

std::string Config::get_run_mode() const {
    switch (runMode) {
    case RunMode::UserFirstLogonAdmin:
        return "UserFirstLogonAdmin (0)";
    case RunMode::SetupComplete:
        return "SetupComplete (1)";
    case RunMode::UserFirstLogonUser:
        return "UserFirstLogonUser (2)";
    case RunMode::UserFirstLogonTrustedInstaller:
        return "UserFirstLogonTrustedInstaller (3)";
    case RunMode::WinDeploy:
        return "WinDeploy (4)";
    default:
        return "Unknown (-1)";
    }
}