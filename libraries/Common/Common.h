#ifndef COMMON_H
#define COMMON_H

inline constexpr uint32_t BAUD_RATE = 19200;

enum Exception : uint8_t
{
    NONE, 
    TIMEOUT,
    INVALID_GET,
    INVALID_SET,
    INVALID_ROUTINE
};

enum Flags : uint8_t
{
    STRIKE,
    DETONATE,
    SOLVE
};

#endif