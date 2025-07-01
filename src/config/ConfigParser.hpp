#pragma once
#include "Config.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <vector>

class ConfigParser {
public:
    explicit ConfigParser(const std::filesystem::path& config_file);
    explicit ConfigParser(const std::string& json_value);
    [[nodiscard]] std::vector<Config> get_configs() const;
    [[nodiscard]] std::vector<Config> take_configs(RunMode mode) const;

private:
    nlohmann::json m_jsonData;
    static Config parse_single_config(const nlohmann::json& json);
};