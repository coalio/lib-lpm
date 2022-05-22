#pragma once
#include <iostream>

#define LPM_PROMPT(msg, var) \
    std::cout << msg << ": "; \
    std::cin >> var;

#define LPM_PACKAGES_MANIFEST_NAME "packages.toml"
#define LPM_PACKAGE_MANIFEST_NAME "package.toml"
#define LPM_DEFAULT_LOCAL_PACKAGES_PATH "lua_modules/${module_name}"
#define LPM_DEFAULT_LOCAL_PACKAGES_BIN "lua_modules/.modules/${binary_name}"
#define LPM_DEFAULT_LOCAL_PACKAGES_INTEGRITY "lua_modules/.modules/module_integrity.toml"

#define LPM_FIND_MAX_DEPTH 10

#ifndef LPM_SHOULD_PRINT_ERRORS
    #define LPM_SHOULD_PRINT_ERRORS 1
#endif

#ifndef LPM_DEBUG_MODE
    #define LPM_DEBUG_MODE 1
#endif

#ifndef LPM_SILENT
    #define LPM_SILENT 0
#endif

#if !LPM_SILENT
    #if LPM_SHOULD_PRINT_ERRORS
        #define LPM_PRINT_ERROR(msg) std::cerr << msg << std::endl
    #else
        #define LPM_PRINT_ERROR(msg)
    #endif

    #if LPM_DEBUG_MODE
        #define LPM_PRINT_DEBUG(msg) std::cerr << msg << std::endl
    #else
        #define LPM_PRINT_DEBUG(msg)
    #endif

    #define LPM_PRINT(msg) std::cout << msg << std::endl
#else
    #define LPM_PRINT_ERROR(msg)
    #define LPM_PRINT_DEBUG(msg)
    #define LPM_PRINT(msg)
#endif

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #define LPM_PATH_SEPARATOR "/"
#else // Windows
    #define LPM_PATH_SEPARATOR "\\"
#endif

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #define LPM_PATH_VAR_SEPARATOR ':'
#else // Windows
    #define LPM_PATH_VAR_SEPARATOR ';'
#endif

// Define paths for Windows, Linux and Unix-like systems

#if defined(__windows__) || defined(_WIN32)
    #define LPM_DEFAULT_ROOT "${USERPROFILE}" LPM_PATH_SEPARATOR ".lpm"
#else
    #define LPM_DEFAULT_ROOT "${HOME}" LPM_PATH_SEPARATOR ".lpm"
#endif

#if defined(__windows__) || defined(_WIN32)
    // TODO: Make LPM::Env::fill_env_vars work on Windows
    #define LPM_CONFIG_PATHS                                                        \
        "${LPM_ROOT}" LPM_PATH_SEPARATOR "lpm.toml",                                \
        "${USERPROFILE}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",   \
        "lpm.toml"
#elif defined(__APPLE__)
    #define LPM_CONFIG_PATHS                                                        \
        "${LPM_ROOT}" LPM_PATH_SEPARATOR "lpm.toml",                                \
        "${HOME}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",          \
        "/Users/${USER}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",   \
        "./lpm.toml"
#else // Linux/Unix
    #define LPM_CONFIG_PATHS                                                        \
        "${LPM_ROOT}" LPM_PATH_SEPARATOR "lpm.toml",                                \
        "${HOME}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",          \
        "/home/${USER}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",    \
        "./lpm.toml"
#endif

// Zip buffer size when reading from zip files
#define LPM_ZIP_BUFFER_SIZE 1024

// The agent used for curl requests performed by LPM for this version
#define LPM_REQUESTS_AGENT = "lpm-agent/dev-0.1"

// LPM doesn't support Lua versions prior to 5.1
#define LPM_LUA_VERSION_TUPLES \
    {                          \
        "Lua5.1",              \
        "lua5.1",              \
        ""                     \
    },                         \
    {                          \
        "Lua5.2",              \
        "lua5.2",              \
        ""                     \
    },                         \
    {                          \
        "Lua5.3",              \
        "lua5.3",              \
        ""                     \
    },                         \
    {                          \
        "Lua5.4",              \
        "lua5.4",              \
        ""                     \
    },                         \
    {                          \
        "LuaJIT",              \
        "luajit",              \
        ""                     \
    },                         \
    {                          \
        "default",             \
        "lua",                 \
        ""                     \
    }
