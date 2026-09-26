#pragma once
#include "controllers/VideoController.hpp"
#include "managers/AudioManager.hpp"
#include "views/BaseView.hpp"

class VideoView : public BaseView
{
  public:
    VideoView(const char* filename) : filename(filename), nextView(ViewState::MAIN_MENU)
    {
    }
    VideoView(const char* filename, const ViewState nextView) : filename(filename), nextView(nextView)
    {
    }
    void init() override;
    ViewState update() override;
    void cleanup() override;

  private:
    const char* filename;
    const ViewState nextView;
    VideoController* videoCtrl = VideoController::getInstance();
    AudioManager& audio = AudioManager::GetInstance();
};
