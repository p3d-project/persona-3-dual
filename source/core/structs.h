/**
 * @file structs.h
 * @brief Holds structs used game-wide
 */

#pragma once
#include "core/enums.h"
#include "core/geometry.h"
#include <etl/vector.h>
#include <nds.h>
#include <string>
#include <vector>

class UIMenu;
class TextComponent;

struct SpriteDBEntry
{
    SpriteType type;
    int id;
    const char* filename;
};

//a simple sprite structure
struct Sprite
{
    u16* gfx;
    SpriteSize size;
    SpriteColorFormat format;
    int rotationIndex;
    int paletteAlpha;
    int x;
    int y;
};

struct MenuOption
{
    const char* name;
    int bgIndex;
    ViewState (UIMenu::*onSelect)();
};

struct MenuState
{
    MenuOption* options;
    int optionCount;
    int selectedOption;
    int startIndex;
};

struct Keyframe
{
    int time;
    s16 rotX, rotY, rotZ;
    s16 posX, posY, posZ;
};

struct AnimTrack
{
    std::vector<Keyframe> frames;
};

struct Animation
{
    int duration;
    std::vector<AnimTrack> nodeTracks;
};

struct SubList
{
    int texSlot;
    std::vector<u32> displayList;
    u32 displayListSize;
};

struct EnvironmentTexture
{
    const char* name;
    int width;
    int height;

    const unsigned int* bitmap;
};

struct BillboardData
{
    const char* name;
    v16 x, y, z;
    v16 halfWidth;
    v16 halfHeight;

    int texSlot;

    short u0, v0;
    short u1, v1;
};

struct EnvironmentDbEntry
{
    // Name/debugging
    const char* name;

    // Binary display list file
    const char* binaryFile;

    // World bounds
    float worldOffsetX;
    float worldOffsetZ;
    float worldWidth;
    float worldDepth;

    // Texture information
    int textureCount;
    const EnvironmentTexture* textures;

    // Billboards
    int billboardCount;
    const BillboardData* billboards;
};

struct AnimNode
{
    int id;
    int parentId;
    std::vector<SubList> subLists;
    std::vector<int> children;
    v16 pivotX, pivotY, pivotZ;
};

struct CharacterPosition
{
    float x;
    float z;
    float y; // height
    float facingAngle;
};

// From DialogueController.h
struct Dialogue;
struct DialogueSelection
{
    std::string text;
    bool isSelected;
    Dialogue* next;
};
struct Dialogue
{
    std::string characterName;
    std::string text;
    int imageId;
    Dialogue* prev;
    Dialogue* next;
    std::vector<DialogueSelection> selections;
};

struct Save
{
    char introVideoPath[128];
    char lastName[32];
    char firstName[32];
    bool femcMode;
} __attribute__((packed));

struct GraphicAsset
{
    int id;
    void* tiles = nullptr;
    u32 tilesLen;
    void* pal = nullptr;
    u32 palLen;
    void* map = nullptr;
    u32 mapLen;
};

/**
 * @brief A single keyframe in a camera path.
 *
 * @see See CameraPath
 */
struct CameraKeyframe
{
    int time;           ///< Frame index at which this keyframe is reached.
    Vec3<float> eye;    ///< Camera eye position.
    Vec3<float> target; ///< Look-at position.
};

/**
 * @brief An ordered list of keyframes defining a camera animation.
 *
 * The camera interpolates linearly between consecutive keyframes.
 * On completion the @ref CameraSystem switches to Follow mode.
 */
struct CameraPath
{
    etl::vector<CameraKeyframe, 100> keyframes;
};

/**
 * @brief A struct that holds initial config values for the MovementComponent
 */
struct MovementConfig
{
    // 3D environment
    int mapWidth;
    int mapHeight;
    uint16_t* collisionMap;

    // world
    float tileSize;
    float worldOffsetX;
    float worldOffsetZ;
    Point2D<float> characterSize;

    // translation
    float speed;
    float height;
    Point2D<float> characterTranslate;
    float characterFacingAngle;

    MovementConfig() = default;

    MovementConfig(int iMapWidth,
                   int iMapHeight,
                   uint16_t* iCollisionMap,
                   float iTileSize,
                   float iWorldOffsetX,
                   float iWorldOffsetZ,
                   Point2D<float> iCharacterSize,
                   float iSpeed,
                   float iHeight,
                   Point2D<float> iCharacterTranslate,
                   float iCharacterFacingAngle)
        : mapWidth(iMapWidth), mapHeight(iMapHeight), collisionMap(iCollisionMap), tileSize(iTileSize),
          worldOffsetX(iWorldOffsetX), worldOffsetZ(iWorldOffsetZ), characterSize(iCharacterSize), speed(iSpeed)
    {
        height = iHeight;
        characterTranslate = iCharacterTranslate;
        characterFacingAngle = iCharacterFacingAngle;
    };
};

/**
 * @brief A struct that holds initial config values for the DialogueComponent
 */
struct DialogueConfig
{
    Dialogue* firstLine = nullptr;
    void (*loader)(int bgIndex) = nullptr;
    TextComponent* text = nullptr;

    DialogueConfig() = default;

    DialogueConfig(Dialogue* iFirstLine, void (*iLoader)(int bgIndex), TextComponent* iText)
        : firstLine(iFirstLine), loader(iLoader), text(iText)
    {
    }
};

/**
 * @brief Stores data for a single glyph (character) in a font.
 */
struct Glyph
{
    int xPos;
    int yPos;
    int width = 0; /// Used to check if the glyph was read in correctly. Setting it to 0 here wipes any old data
    int height;
    int xOffset;
    int yOffset;
};

/**
 * @brief Stores data for a font.
 * @note Assumes that the regular and bold (if present) font bitmaps are the same size.
 */
struct Font
{
    std::uint8_t* bitmap = nullptr;
    std::uint8_t* bitmapBold = nullptr;
    int bitmapWidth = 256;
    int bitmapHeight = 256;
    int lineHeight = 32;
    Glyph glyphs[256];
    Glyph boldGlyphs[256];
    bool boldLoaded = false;
};

/**
 * @brief A struct that represents a block of text being rendered on the screen.
 */
struct Text
{
    int cursorX;
    int cursorY;
    int startX;
    int startY;
    std::string content;
    Font* font;
    uint16_t* videoBuffer;
    int cursorPos;
    int baseColor;
    int activeColor;
    int counter;
    bool bold;
    bool italic;
    bool underline;
};

/**
 * @brief A struct that holds initial config values for the TextComponent
 */
struct TextConfig
{
    uint16_t* videoBuffer = nullptr;

    /// for loadFont
    std::string* fontNamePath = nullptr;
    int fontSize;

    /// for loadFontBitmap
    std::string* fontBitmapPath = nullptr;

    /// for loadPalette
    std::string* fontPalettePath = nullptr;
    bool isSub;

    /// for loadFontMetadata
    std::string* fontMetadataPath = nullptr;
    bool isBoldBitmap;

    TextConfig() = default;

    /// load font (loads font, font bitmap, font metadata. NOT font palette)
    TextConfig(uint16_t* iVideoBuffer, std::string* iFontNamePath, int iFontSize)
        : videoBuffer(iVideoBuffer), fontNamePath(iFontNamePath), fontSize(iFontSize)
    {
    }

    /// load font palette
    TextConfig(uint16_t* iVideoBuffer, std::string* iFontPalettePath, bool iIsSub)
        : videoBuffer(iVideoBuffer), fontPalettePath(iFontPalettePath), isSub(iIsSub)
    {
    }
};
