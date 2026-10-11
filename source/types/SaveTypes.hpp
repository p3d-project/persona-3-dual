#pragma once

struct GameVersion
{
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint8_t patchVersion;
    bool operator==(const GameVersion& other) const
    {
        return majorVersion == other.majorVersion && minorVersion == other.minorVersion &&
               patchVersion == other.patchVersion;
    }
    bool operator!=(const GameVersion& other) const
    {
        return majorVersion != other.majorVersion || minorVersion != other.minorVersion ||
               patchVersion != other.patchVersion;
    }
} __attribute__((packed));

struct SaveHeader
{
    GameVersion version;
    // add save checksum
} __attribute__((packed));

struct Save
{
    SaveHeader header;
    char lastName[32];
    char firstName[32];
} __attribute__((packed));
