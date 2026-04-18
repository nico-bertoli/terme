#pragma once

// settings
#define DEBUG 1

namespace terme::settings
{
    static constexpr bool kDebug = DEBUG;
    static constexpr bool kAllowSounds = false;
    static constexpr const char* kDebugLogFilePath = "logs.txt";
}













// ------------------------------------------------
// platform
#if defined(_WIN32) || defined(_WIN64)
    #define IS_LINUX_BUILD 0
#elif defined(__linux__)
    #define IS_LINUX_BUILD 1
#else
    #define IS_LINUX_BUILD 1
    #warning "Unknown platform detected, defaulting to Linux build. Please verify platform detection."
#endif