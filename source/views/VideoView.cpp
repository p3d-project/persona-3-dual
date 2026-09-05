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
    if ((systemKeysDown & KEY_A) || (systemKeysDown & KEY_START) || (systemKeysDown & KEY_TOUCH))
    {
        musicCtrl->pause();
        for (int i = 0; i <= 16; i++)
        {
            setBrightness(3, -i);
            musicCtrl->update();
            swiWaitForVBlank();
        }

        return nextView;
    }

    return videoCtrl->update();
}

void VideoView::cleanup()
{
    videoCtrl->cleanup();
    BaseView::cleanup();
}
