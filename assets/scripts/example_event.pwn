#include <native>

main()
{
    new CameraMode:ogCameraMode = GET_CAMERA_MODE();

    WAIT(240);
    SET_CAMERA_MODE(CAMERA_FREE);
    WAIT(240);
    SET_CAMERA_MODE(CAMERA_STATIC);
    WAIT(240);
    SET_CAMERA_MODE(CAMERA_CCTV);
    WAIT(240);
    SET_CAMERA_MODE(ogCameraMode);
}
