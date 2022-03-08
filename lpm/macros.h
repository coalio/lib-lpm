#pragma once
#include <iostream>

#define LPM_PACKAGES_MANIFEST_NAME "packages.toml"
#define LPM_MODULE_MANIFEST_NAME "module.toml"
#define LPM_DEFAULT_LOCAL_MODULES_PATH "lpm_modules/${module_name}"
#define LPM_DEFAULT_LOCAL_MODULES_BIN "lpm_modules/.modules/${binary_name}"
#define LPM_DEFAULT_LOCAL_MODULES_INTEGRITY "lpm_modules/.modules/module_integrity.toml"

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

// Define paths for Linux and Unix-like systems
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #define LPM_PATH_SEPARATOR "/"
#else // Windows
    #define LPM_PATH_SEPARATOR "\\"
#endif

#if defined(__windows__) || defined(_WIN32)
    // TODO: Make LPM::Env::fill_env_vars work on Windows
    #define LPM_CONFIG_PATHS                                                       \
        "%LPM_ROOT%" LPM_PATH_SEPARATOR "lpm.toml",                                \
        "%USERPROFILE%" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",   \
        "lpm.toml"
#elif defined(__APPLE__)
    #define LPM_CONFIG_PATHS                                                       \
        "${LPM_ROOT}" LPM_PATH_SEPARATOR "lpm.toml",                               \
        "${HOME}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",         \
        "/Users/${USER}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",  \
        "./lpm.toml"
#else // Linux/Unix
    #define LPM_CONFIG_PATHS                                                       \
        "${LPM_ROOT}" LPM_PATH_SEPARATOR "lpm.toml",                               \
        "${HOME}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",         \
        "/home/${USER}" LPM_PATH_SEPARATOR ".lpm" LPM_PATH_SEPARATOR "lpm.toml",   \
        "./lpm.toml"
#endif

// Specific macros

#define LPM_ZIP_BUFFER_SIZE 1024