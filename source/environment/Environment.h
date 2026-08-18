#pragma once
#include <array>
#include <nds.h>

#include "data/environmentDb.h"

// Largest textureCount across all current g_environmentDb entries (dorm = 32).
// Bump this if a future room introduces more textures than that.
constexpr int MAX_ENVIRONMENT_TEXTURES = 32;

// One generic runtime representation of a room's 3D geometry
class Environment
{
  public:
    /**
     * @brief Constructs an empty Environment with all texture and display-list
     *        slots cleared and no database entry attached.
     */
    Environment();

    /**
     * @brief Loads environment geometry (display lists) and textures from the
     *        compiled .bin file described by an EnvironmentDbEntry.
     *
     * Always begins with a hard reset (cleanup()) of any previously loaded
     * state, so this is safe to call repeatedly across room transitions
     * without leaking display lists or texture slots. On any failure the
     * partially loaded state is cleaned up again before returning.
     *
     * @param entry   Pointer to the database entry describing this environment
     *                (name, texture metadata/count, binary file name). Must be
     *                non-null and have textureCount <= MAX_ENVIRONMENT_TEXTURES,
     *                otherwise this fails immediately.
     * @param bitmaps Array of raw bitmap pointers, one per texture slot, to be
     *                uploaded to VRAM. Max size of MAX_ENVIRONMENT_TEXTURES.
     *                May be null, or contain null entries, in which case the
     *                corresponding texture slot(s) are skipped.
     * @return true if the binary file was opened, validated, and fully loaded
     *         (display lists and textures); false if @p entry was invalid, the
     *         file could not be opened or failed magic/size validation, or an
     *         allocation failed while reading display list data.
     *
     * @note entry->binaryFile is a build-time string baked in by
     *       obj2environment.py. It cannot know the runtime fat/SD mount root, so
     *       it must be combined with fatBasePath before fopen() can find it.
     *       ASSUMPTION: binaryFile already contains the full
     *       "environments/<name>/" relative path. If the printed path below is
     *       missing that folder, binaryFile is actually the bare filename and this
     *       needs to be fatBasePath + "environments/" + entry->name + "/" +
     *       entry->binaryFile instead. Check environmentDb.cpp's entry to
     *       confirm behaviour.
     */
    bool load(const EnvironmentDbEntry* entry, std::array<const unsigned int*, MAX_ENVIRONMENT_TEXTURES> bitmaps);

    /**
     * @brief Renders every loaded texture/display-list pair for the currently
     *        loaded environment.
     *
     * Does nothing if no environment is currently loaded (dbEntry is null).
     * Skips any texture slot that has no bound texture ID, and skips the draw
     * call itself (but still binds the texture) if that slot has no display
     * list, guarding against corrupted or missing display-list pointers.
     */
    void draw();

    /**
     * @brief Renders every billboard quad defined for the currently loaded
     *        environment
     *
     * @note  Batches consecutive billboards that share a texture slot into a
     *        single glBegin/glEnd(GL_QUADS) block. Does nothing if no
     *        environment is loaded or it has no billboards. Any billboard
     *        referencing an out-of-range or unbound texture slot is skipped.
     *
     * @param faceCamera If true, each billboard's right vector is recomputed
     *                    every call so it always faces the given camera
     *                    position (classic screen-facing billboarding). If
     *                    false, billboards use a fixed world-axis-aligned
     *                    orientation.
     * @param camX World-space X coordinate of the camera, used only when
     *             @p faceCamera is true.
     * @param camY World-space Y coordinate of the camera (unused. billboards
     *             only rotate about the Y axis).
     * @param camZ World-space Z coordinate of the camera, used only when
     *             @p faceCamera is true.
     */
    void drawBillboards(bool faceCamera, float camX, float camY, float camZ);

    /**
     * @brief Frees all display lists and deletes all textures owned by
     *        this Environment, and clears the current database entry.
     *
     * Safe to call at any time, including when no environment is loaded
     *
     * @note Always sweeps the full fixed-size arrays rather than looping to
     *       dbEntry->textureCount. Looping to the current entry's count
     *       would mean a load() that failed partway through (or a room
     *       with fewer texture slots than the one loaded before it) could
     *       leave stale, un-freed pointers sitting past that count, silently
     *       leaking memory and VRAM texture slots across room transitions.
     */
    void cleanup();

  private:
    const EnvironmentDbEntry* dbEntry;

    u32* displayLists[MAX_ENVIRONMENT_TEXTURES];
    u32 dlSizes[MAX_ENVIRONMENT_TEXTURES];
    int textureIDs[MAX_ENVIRONMENT_TEXTURES];
};
