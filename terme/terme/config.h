#pragma once

// settings
#define ALLOW_SOUNDS 1
#define DEBUG 1

// debug details
#define SHOW_COUT_CALLS 1
#define SHOW_FPS 1

// cheats
#define CHEAT_SPACEINV_FAST_FIRE 0
#define CHEAT_SPACEINV_DISABLE_ENEMY_FIRE 0
#define CHEAT_SPACEINV_DISABLE_SHIELDS 0
#define CHEAT_SPACEINV_INVINCIBILITY 0

// autodetect platform
#if defined(_WIN32) || defined(_WIN64)
    #define IS_LINUX_BUILD 0
#elif defined(__linux__)
    #define IS_LINUX_BUILD 1
#else
    #define IS_LINUX_BUILD 1
    #warning "Unknown platform detected, defaulting to Linux build. Please verify platform detection."
#endif