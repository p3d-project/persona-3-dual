#include "VideoView.hpp"
#include "core/globals.hpp"
#include <nds.h>

void VideoView::init()
{
    videoCtrl->init(filename, ae::q20_12_t{15.0}, nextView);
    setBrightness(2, -16);
}

ViewState VideoView::update()
{
    switch (transitionPhase)
    {
    case TransitionPhase::IDLE:
    {
        // Check if user wants to skip
        if ((systemKeysDown & KEY_A) || (systemKeysDown & KEY_START) || (systemKeysDown & KEY_TOUCH))
        {
            audio.pauseAudio();
            fadeTimer.start(ae::q20_12_t{0.3});
            transitionPhase = TransitionPhase::FADING_OUT;
            break;
        }

        return videoCtrl->update();
    }

    case TransitionPhase::FADING_OUT:
    {
        if (fadeTimer.isFinished())
        {
            setBrightness(3, -16);
            return nextView;
        }
        else
        {
            int fadeVal = fadeTimer.getProgress().raw_value() >> 8;
            setBrightness(3, -fadeVal);
        }
        break;
    }
    }

    return ViewState::KEEP_CURRENT;
}

void VideoView::cleanup()
{
    videoCtrl->cleanup();
    BaseView::cleanup();
}
