cmake_minimum_required(VERSION 3.20)
project(dpi_c2_detector VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Policy minimum for 3rd-party subprojects (e.g. PcapPlusPlus bundled deps on CMake 4+)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

# async logging
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.3
)
FetchContent_MakeAvailable(spdlog)

# json alerts
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# ppp (network parsing)
set(PCAPPP_BUILD_TESTS OFF CACHE BOOL "Disable PcapPlusPlus tests" FORCE)
set(PCAPPP_BUILD_EXAMPLES OFF CACHE BOOL "Disable PcapPlusPlus examples" FORCE)
FetchContent_Declare(
        PcapPlusPlus
        GIT_REPOSITORY https://github.com/seladb/PcapPlusPlus.git
        GIT_TAG        v24.09
)
FetchContent_MakeAvailable(PcapPlusPlus)

# Disable -Werror on third-party PcapPlusPlus targets (GCC 15 false-positive in Asn1Codec)
set_target_properties(Common++ Packet++ Pcap++ PROPERTIES COMPILE_WARNING_AS_ERROR OFF)

# Target Definition
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/ingestion/PcapReader.cpp
)

# to ensure c++ std and no default compiler extensions
target_compile_options(${PROJECT_NAME}
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -O3
        -march=native
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Pcap++
        Packet++
        Common++
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)
