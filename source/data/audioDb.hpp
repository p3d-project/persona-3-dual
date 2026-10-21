#pragma once
#include "types/aeTypes.hpp"
#include <string>

enum class AudioType
{
    NONE = 0,
    BATTLE,
    LOCATION,
    MENU,
    OTHER,
    // SFX_AUDIO,
    CUSTOM,
};

struct AudioDBEntry
{
    AudioType type;
    int id;
    const char* filename;
    ae::q20_12_t loopStartTime;
    ae::q20_12_t loopEndTime;
};

enum class BattleAudio
{
    MASS_DESTRUCTION = 0,
};

enum class LocationAudio
{
    IWATODAI_DORM = 0,
    SUMMER_BOUND,
    COLOR_YOUR_NIGHT,
    PAULOWNIA_MALL,
};

enum class MenuAudio
{
    MISTIC = 0,
    TIGHTROPE,
    ARIA_OF_THE_SOUL,
};

enum class OtherAudio
{
    AFTER_THE_BATTLE = 0,
    BECAUSE_I_WILL_PROTECT_YOU,
    CLUB_ESCAPADE_NOUVEAU_METALMOTH_REMIX,
};

// TODO: add SFX audio
// enum class SFXAudio
// {
// };

/**
 * @brief Get the audio file path
 *
 * @param type th AudioType to get
 * @param id the specific audio to get
 * @return std::string the filepath
 */
std::string getAudioFilepath(AudioType type, int id);

/**
 * @brief Get the loop start time
 *
 * @param type th AudioType to get
 * @param id the specific audio to get
 * @return ae::q20_12_t the loop end time
 */
ae::q20_12_t getLoopStart(AudioType type, int id);

/**
 * @brief Get the loop end time
 *
 * @param type th AudioType to get
 * @param id the specific audio to get
 * @return ae::q20_12_t the loop end time
 */
ae::q20_12_t getLoopEnd(AudioType type, int id);

/**
 * @brief Internal helper to get the AudioDBEntry struct
 *
 * @param type th AudioType to get
 * @param id the specific audio to get
 * @return AudioDBEntry the data
 */
AudioDBEntry getAudioDBEntry(AudioType type, int id);
