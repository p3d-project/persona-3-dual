#pragma once
#include <cstdint>

struct GameVersion
{
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint8_t patchVersion;
    bool operator==(const GameVersion& other) const = default;
} __attribute__((packed));

struct SaveHeader
{
    GameVersion version;
    // add save checksum
} __attribute__((packed));

struct Save
{
    SaveHeader header;
    char lastName[10];
    char firstName[10];
} __attribute__((packed));
