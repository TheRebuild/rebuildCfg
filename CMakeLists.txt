cmake_minimum_required(VERSION 3.31)
project(rebuildCfg)

set(CMAKE_CXX_STANDARD 23)

add_executable(rebuildCfg src/main.cpp
        src/config/Config.hpp
        src/config/ConfigParser.hpp
        src/config/ConfigParser.cpp)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)
target_link_libraries(rebuildCfg PRIVATE nlohmann_json::nlohmann_json)