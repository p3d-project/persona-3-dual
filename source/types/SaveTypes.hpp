#pragma once

struct GameVersion
{
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint8_t patchVersion;
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
