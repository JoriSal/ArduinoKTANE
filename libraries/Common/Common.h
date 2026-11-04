#ifndef COMMON_H
#define COMMON_H

inline constexpr uint32_t BAUD_RATE = 19200;

enum Exceptions : uint8_t
{
    INVALID_GET
};

enum Flags : uint8_t
{
    STRIKE
};

#endif