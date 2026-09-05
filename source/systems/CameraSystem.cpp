#include "CameraSystem.hpp"
#include "core/globals.hpp"

void CameraSystem::on_receive(const Event::ConfigureCamera& config)
{
    isActive = true;
    mode = config.mode;
    currentPos = config.eye;
    targetPos = config.target;
    angle = config.initialAngle;
    isRotationLocked = config.isRotationLocked;
    distance = config.distance;
    height = config.height;
    lookAhead = config.lookAhead;
    angleIncrement = config.angleIncrement;
}

void CameraSystem::on_receive(const Event::SetCameraMode& msg)
{
    mode = msg.mode;
    if (msg.mode == CameraMode::Path)
    {
        pathFrame = 0;
        pathKeyIndex = 0;
        pathDone = false;
    }
    if (msg.mode == CameraMode::Free)
    {
        freeInitialised = false;
    }
}

void CameraSystem::on_receive(const Event::SetCameraPath& msg)
{
    path = msg.path;
    pathFrame = 0;
    pathKeyIndex = 0;
    pathDone = false;
}

void CameraSystem::on_receive(const Event::SetCharacterPosition& msg)
{
    charPos = msg.charPos;
}

void CameraSystem::on_receive(const Event::StartCamera& msg)
{
    isActive = true;
}

void CameraSystem::on_receive(const Event::StopCamera& msg)
{
    isActive = false;
}

ae::q20_12_t CameraSystem::getMovementAngle() const
{
    switch (mode)
    {
    case CameraMode::CCTV:
    case CameraMode::Static:
        return MathManager::GetInstance().atan2(ae::q20_12_t{currentPos.x - charPos.x},
                                                ae::q20_12_t{charPos.z - currentPos.z});
    default:
        return angle;
    }
}

void CameraSystem::Init()
{
    isActive = false;
}

void CameraSystem::Shutdown()
{
    isActive = false;
}

void CameraSystem::Update(ae::q20_12_t)
{
    camPos.up.y = ae::q20_12_t{1};

    switch (mode)
    {
    case CameraMode::Static:
    {
        camPos.eye.x = currentPos.x;
        camPos.eye.y = currentPos.y;
        camPos.eye.z = currentPos.z;
        camPos.target.x = targetPos.x;
        camPos.target.y = targetPos.y;
        camPos.target.z = targetPos.z;
        break;
    }

    case CameraMode::CCTV:
    {
        camPos.eye.x = currentPos.x;
        camPos.eye.y = currentPos.y;
        camPos.eye.z = currentPos.z;
        camPos.target.x = charPos.x;
        camPos.target.y = charPos.y;
        camPos.target.z = charPos.z;
        break;
    }

    case CameraMode::Follow:
    {
        if (!isRotationLocked && (systemKeysHeld & KEY_L))
        {
            angle -= angleIncrement;
        }

        if (!isRotationLocked && (systemKeysHeld & KEY_R))
        {
            angle += angleIncrement;
        }

        camPos.eye.x = charPos.x + ae::q20_12_t{math.sin(angle)} * distance;
        camPos.eye.y = charPos.y + height;
        camPos.eye.z = charPos.z - ae::q20_12_t{math.cos(angle)} * distance;

        camPos.target.x = charPos.x - ae::q20_12_t{math.sin(angle)} * lookAhead;
        camPos.target.y = charPos.y + ae::q20_12_t{0.1};
        camPos.target.z = charPos.z + ae::q20_12_t{math.cos(angle)} * lookAhead;
        break;
    }

    case CameraMode::Free:
    {
        if (!freeInitialised)
        {
            currentPos.x = charPos.x;
            currentPos.y = charPos.y + height;
            currentPos.z = charPos.z;
            freeInitialised = true;
        }

        if (!isRotationLocked && (systemKeysHeld & KEY_L))
        {
            angle -= angleIncrement;
        }

        if (!isRotationLocked && (systemKeysHeld & KEY_R))
        {
            angle += angleIncrement;
        }

        const ae::q20_12_t fwdX = -ae::q20_12_t{math.sin(angle)} * freeCameraSpeed;
        const ae::q20_12_t fwdZ = ae::q20_12_t{math.cos(angle)} * freeCameraSpeed;

        if (systemKeysHeld & KEY_UP)
        {
            currentPos.x += fwdX;
            currentPos.z += fwdZ;
        }
        if (systemKeysHeld & KEY_DOWN)
        {
            currentPos.x -= fwdX;
            currentPos.z -= fwdZ;
        }
        if (systemKeysHeld & KEY_RIGHT)
        {
            currentPos.x -= fwdZ;
            currentPos.z += fwdX;
        }
        if (systemKeysHeld & KEY_LEFT)
        {
            currentPos.x += fwdZ;
            currentPos.z -= fwdX;
        }

        camPos.eye.x = currentPos.x;
        camPos.eye.y = currentPos.y;
        camPos.eye.z = currentPos.z;
        camPos.target.x = currentPos.x - ae::q20_12_t{math.sin(angle)};
        camPos.target.y = currentPos.y;
        camPos.target.z = currentPos.z + ae::q20_12_t{math.cos(angle)};

        break;
    }

    case CameraMode::Path:
    {
        if (!path || path->keyframes.size() < 2)
            break;

        pathFrame++;

        while (pathKeyIndex + 2 < static_cast<int>(path->keyframes.size()) &&
               pathFrame >= path->keyframes[pathKeyIndex + 1].time)
        {
            pathKeyIndex++;
        }

        const CameraKeyframe& kf0 = path->keyframes[pathKeyIndex];
        const CameraKeyframe& kf1 = path->keyframes[pathKeyIndex + 1];

        if (pathFrame >= kf1.time && pathKeyIndex + 2 >= static_cast<int>(path->keyframes.size()))
        {
            pathDone = true;
            mode = CameraMode::Follow;
            camPos.eye.x = kf1.eye.x;
            camPos.eye.y = kf1.eye.y;
            camPos.eye.z = kf1.eye.z;
            camPos.target.x = kf1.target.x;
            camPos.target.y = kf1.target.y;
            camPos.target.z = kf1.target.z;
            break;
        }

        int span = kf1.time - kf0.time;
        ae::q20_12_t t =
            (span > 0) ? math.div(ae::q20_12_t{pathFrame - kf0.time}, ae::q20_12_t{span}) : ae::q20_12_t{1};

        camPos.eye.x = kf0.eye.x + (kf1.eye.x - kf0.eye.x) * t;
        camPos.eye.y = kf0.eye.y + (kf1.eye.y - kf0.eye.y) * t;
        camPos.eye.z = kf0.eye.z + (kf1.eye.z - kf0.eye.z) * t;
        camPos.target.x = kf0.target.x + (kf1.target.x - kf0.target.x) * t;
        camPos.target.y = kf0.target.y + (kf1.target.y - kf0.target.y) * t;
        camPos.target.z = kf0.target.z + (kf1.target.z - kf0.target.z) * t;
        break;
    }

    default:
        break;
    }

    ae::BroadcastEvent(camPos);
}
