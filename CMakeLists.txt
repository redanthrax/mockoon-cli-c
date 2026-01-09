cmake_minimum_required(VERSION 3.10)
project(mockoon-cli-c
        VERSION 1.0.0
        DESCRIPTION "Mockoon CLI rewritten in C"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find required packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JANSSON REQUIRED jansson)

# Optional packages
find_package(yaml)

# Source files
set(SOURCES
    src/main.c
    src/utils/config.c
    src/utils/colors.c
    src/utils/args.c
    src/core/file.c
    src/core/json.c
    src/core/http.c
    src/core/schema.c
    src/core/openapi.c
    src/core/watcher.c
    src/commands/validate.c
    src/commands/import.c
    src/commands/export.c
    src/commands/start.c
    src/commands/dockerize.c
)

# Create executable
add_executable(mockoon-cli ${SOURCES})

# Link libraries
target_link_libraries(mockoon-cli PRIVATE
    CURL::libcurl
    ${JANSSON_LIBRARIES}
    Threads::Threads
)
target_include_directories(mockoon-cli PRIVATE ${JANSSON_INCLUDE_DIRS})

if(yaml_FOUND)
    target_link_libraries(mockoon-cli PRIVATE yaml)
endif()

# Installation
install(TARGETS mockoon-cli
        RUNTIME DESTINATION bin)

# Testing
enable_testing()
add_subdirectory(test)
