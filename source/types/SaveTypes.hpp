#pragma once

struct Save
{
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint8_t patchVersion;
    char lastName[32];
    char firstName[32];
} __attribute__((packed));
