#pragma once
#include <optional>
#include <string>

enum class RunMode {
    UserFirstLogonAdmin = 0,
    SetupComplete = 1,
    UserFirstLogonUser = 2,
    UserFirstLogonTrustedInstaller = 3,
    WinDeploy = 4,
    Unknown = -1
};

struct Config {
    std::string category;
    std::string name;
    std::string filename;
    RunMode runMode = RunMode::UserFirstLogonAdmin;
    std::optional<std::string> additional_args;
    std::optional<std::string> description;

    [[nodiscard]] std::string get_run_mode() const;
};