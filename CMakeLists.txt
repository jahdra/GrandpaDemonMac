cmake_minimum_required(VERSION 3.25)

# Set before project()/add_library(): every mod object must have both Mac slices.
if(CMAKE_HOST_APPLE AND NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures")
endif()
if(CMAKE_HOST_APPLE AND NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
endif()
project(GrandpaDemon VERSION 1.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
option(GRANDPA_BUILD_MOD "Build the Geode mod (requires a supported platform and SDK)" ON)
option(GRANDPA_BUILD_TESTS "Build standalone ranking regression tests" OFF)

if(GRANDPA_BUILD_MOD)
    if(NOT DEFINED ENV{GEODE_SDK} OR NOT EXISTS "$ENV{GEODE_SDK}/CMakeLists.txt")
        message(FATAL_ERROR "Set GEODE_SDK to a Geode 5.10.1 SDK checkout, or use -DGRANDPA_BUILD_MOD=OFF -DGRANDPA_BUILD_TESTS=ON for standalone tests.")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17.0")
        message(FATAL_ERROR "Geode 5.10.1 requires Xcode 26 / Apple Clang 17 or newer. Select a supported Xcode with xcode-select.")
    endif()
    add_library(${PROJECT_NAME} SHARED
        src/main.cpp
        src/LevelInfoLayer.cpp
        src/LevelCell.cpp
        src/DemonFilterSelectLayer.cpp
        src/LevelBrowserLayer.cpp
    )
    add_subdirectory("$ENV{GEODE_SDK}" "${CMAKE_CURRENT_BINARY_DIR}/geode")
    setup_geode_mod(${PROJECT_NAME})
endif()

if(GRANDPA_BUILD_TESTS)
    enable_testing()
    if(NOT TARGET mat-json)
        include(FetchContent)
        FetchContent_Declare(matjson
            GIT_REPOSITORY https://github.com/geode-sdk/json.git
            GIT_TAG v3.3.1
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(matjson)
    endif()
    add_executable(ranking_tests tests/ranking_tests.cpp)
    target_include_directories(ranking_tests PRIVATE src)
    target_link_libraries(ranking_tests PRIVATE mat-json)
    add_test(NAME ranking_tests COMMAND ranking_tests)
endif()
