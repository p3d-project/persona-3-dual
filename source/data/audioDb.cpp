#include "data/audioDb.hpp"

static const AudioDBEntry AUDIO_DB_ENTRY[] = {
    // battle
    {AudioType::BATTLE,
     static_cast<int>(BattleAudio::MASS_DESTRUCTION),
     "battle/mass_destruction",
     ae::q20_12_t{0.000},
     ae::q20_12_t{-1.000}},

    // location
    {AudioType::LOCATION,
     static_cast<int>(LocationAudio::IWATODAI_DORM),
     "locations/iwatodaiDorm/iwatodai_dorm",
     ae::q20_12_t{1.300},
     ae::q20_12_t{-1.000}},
    {AudioType::LOCATION,
     static_cast<int>(LocationAudio::CHANGING_SEASONS),
     "locations/iwatodaiStreets/changing_seasons",
     ae::q20_12_t{0.000},
     ae::q20_12_t{-1.000}}, // TODO: get correct looping points
    {AudioType::LOCATION,
     static_cast<int>(LocationAudio::COLOR_YOUR_NIGHT),
     "locations/paulowniaMall/overworld/color_your_night",
     ae::q20_12_t{0.000},
     ae::q20_12_t{-1.000}}, // TODO: get correct looping points
    {AudioType::LOCATION,
     static_cast<int>(LocationAudio::PAULOWNIA_MALL),
     "locations/paulowniaMall/station/paulownia_mall",
     ae::q20_12_t{0.000},
     ae::q20_12_t{-1.000}}, // TODO: get correct looping points

    // menu
    {AudioType::MENU,
     static_cast<int>(MenuAudio::MISTIC),
     "menus/contract/mistic",
     ae::q20_12_t{0.000},
     ae::q20_12_t{-1.000}},
    {AudioType::MENU,
     static_cast<int>(MenuAudio::TIGHTROPE),
     "menus/title/tightrope",
     ae::q20_12_t{17.962},
     ae::q20_12_t{66.082}},
    {AudioType::MENU,
     static_cast<int>(MenuAudio::ARIA_OF_THE_SOUL),
     "menus/velvetRoom/aria_of_the_soul",
     ae::q20_12_t{0.000},
     ae::q20_12_t{-1.000}},

    // other
    {AudioType::OTHER, static_cast<int>(OtherAudio::AFTER_THE_BATTLE), "other/after_the_battle"},
    {AudioType::OTHER, static_cast<int>(OtherAudio::BECAUSE_I_WILL_PROTECT_YOU), "other/because_i_will_protect_you"},
    {AudioType::OTHER,
     static_cast<int>(OtherAudio::CLUB_ESCAPADE_NOUVEAU_METALMOTH_REMIX),
     "other/club_escapade_nouveau_metalmoth_remix"},
};

static const int AUDIO_DB_ENTRY_LEN = sizeof(AUDIO_DB_ENTRY) / sizeof(AUDIO_DB_ENTRY[0]);

AudioDBEntry getAudioDBEntry(AudioType type, int id)
{
    for (int i = 0; i < AUDIO_DB_ENTRY_LEN; ++i)
    {
        if (AUDIO_DB_ENTRY[i].type == type && AUDIO_DB_ENTRY[i].id == id)
        {
            return AUDIO_DB_ENTRY[i];
        }
    }
    return AudioDBEntry{AudioType::NONE};
}

std::string getAudioFilepath(AudioType type, int id)
{
    AudioDBEntry entry = getAudioDBEntry(type, id);
    if (entry.type == AudioType::NONE)
    {
        return "";
    }

    return entry.filename;
}

ae::q20_12_t getLoopStart(AudioType type, int id)
{
    AudioDBEntry entry = getAudioDBEntry(type, id);
    if (entry.type == AudioType::NONE)
    {
        return ae::q20_12_t{0.000};
    }

    return entry.loopStartTime;
}

ae::q20_12_t getLoopEnd(AudioType type, int id)
{
    AudioDBEntry entry = getAudioDBEntry(type, id);
    if (entry.type == AudioType::NONE)
    {
        return ae::q20_12_t{-1.000};
    }

    return entry.loopEndTime;
}
