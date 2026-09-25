#pragma once

struct Save
{
    const uint8_t majorVersion;
    const uint8_t minorVersion;
    char lastName[32];
    char firstName[32];
} __attribute__((packed));
