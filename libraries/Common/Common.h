#ifndef COMMON_H
#define COMMON_H

inline constexpr uint32_t BAUD_RATE = 19200;

enum Exception : uint8_t
{
    NO_ERROR = 0,
    TIMEOUT,
    INVALID_GET,
    INVALID_SET,
    INVALID_BOMB_SETUP,
    INVALID_ROUTINE
};

enum Flags : uint16_t
{
    STRIKE = 1 << 0,
    DETONATE = 1 << 1,
    SOLVE = 1 << 2
};

enum ModuleType : uint8_t
{
    NO_MODULE = 0,
    NORMAL,
    NEEDY,
    DECORATIVE
};

#endif