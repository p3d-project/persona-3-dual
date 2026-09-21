#pragma once

enum class ViewState
{
    DEFAULT,
    KEEP_CURRENT,
    DISCLAIMER,
    INTRO_VIDEO,
    INTRO,
    MAIN_MENU,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    CUTSCENE_1,
    SIGN_CONTRACT,
    CUTSCENE_2,
    STATION,
    PAULOWNIA_MALL,
};

enum class TransitionPhase
{
    FADING_IN,
    FADING_IN_TOP,
    FADING_IN_SKY,
    IDLE,
    FADING_OUT
};
