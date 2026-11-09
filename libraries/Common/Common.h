#ifndef COMMON_H
#define COMMON_H

inline constexpr uint32_t BAUD_RATE = 19200;

enum Exceptions : uint8_t
{
    TIMEOUT,
    INVALID_GET,
    INVALID_SET
};

enum Flags : uint8_t
{
    STRIKE,
    DETONATE
};

#endif