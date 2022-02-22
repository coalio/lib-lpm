#pragma once
#include <iostream>

#define LPM_DEFAULT_PACKAGES_PATH "packages.toml"

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
