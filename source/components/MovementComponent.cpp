#include "MovementComponent.hpp"
#include "core/globals.h"

// TODO: remove hardcoded model references
#include "models/kotone.h"
#include "models/makoto.h"

// TODO: decouple camera with movement (so that movement can be used by other entities) (make a seperate component?)
// TODO: decouple animation controller with movement (make a seperate component?)

void MovementComponent::Init()
{
    isActive = false;
    walkAnim =
        saveData.femcMode ? (int)MODEL_KOTONE_ROOT_MODEL_MOTION_0002 : (int)MODEL_MAKOTO_PLAYER_ROOT_MODEL_MOTION_0002;
    idleAnim = saveData.femcMode ? (int)MODEL_KOTONE_ROOT_MODEL_MOTION : (int)MODEL_MAKOTO_PLAYER_ROOT_MODEL_MOTION;
}

void MovementComponent::Destroy()
{
    isActive = false;
}

void MovementComponent::Update(ae::fixed_t)
{
    // TODO: don't broadcast on every update (waste cpu cycles). Set it once?
    ae::BroadcastEvent(Event::SetCharacterPosition{isCharacterAt()});
    float cameraAngle = CameraSystem::GetInstance().getMovementAngle();
    float forwardX;
    float forwardZ;
    float rightX;
    float rightZ;

    float deltaX = 0.0f;
    float deltaZ = 0.0f;

    float nextX;
    float nextZ;

    float angleRad;

    forwardX = -math.sin(cameraAngle) * config.speed;
    forwardZ = math.cos(cameraAngle) * config.speed;
    rightX = math.cos(cameraAngle) * config.speed;
    rightZ = math.sin(cameraAngle) * config.speed;

    if (systemKeysHeld & KEY_UP)
    {
        deltaX += forwardX;
        deltaZ += forwardZ;
    }

    if (systemKeysHeld & KEY_DOWN)
    {
        deltaX -= forwardX;
        deltaZ -= forwardZ;
    }

    if (systemKeysHeld & KEY_RIGHT)
    {
        deltaX -= rightX;
        deltaZ -= rightZ;
    }

    if (systemKeysHeld & KEY_LEFT)
    {
        deltaX += rightX;
        deltaZ += rightZ;
    }

    if (deltaX != 0.0f || deltaZ != 0.0f)
    {
        // set walking animation
        if (Globals::enableCharacterAnim && (animationCtrl->getCurrentAnimIndex() != walkAnim))
        {
            animationCtrl->set(walkAnim, true);
        }

        // normalize diagonal movement to prevent faster speed
        if (deltaX != 0.0f && deltaZ != 0.0f)
        {
            const float invSqrt2 = 0.707106781187f;
            deltaX *= invSqrt2;
            deltaZ *= invSqrt2;
        }
    }
    else
    {
        // set idle animation
        if (Globals::enableCharacterAnim && (animationCtrl->getCurrentAnimIndex() != idleAnim))
        {
            animationCtrl->set(idleAnim, true);
        }
    }
    animationCtrl->play();

    nextX = config.characterTranslate.x + deltaX;
    nextZ = config.characterTranslate.z + deltaZ;

    // try full movement first
    if (isTileWalkable(nextX, nextZ))
    {
        config.characterTranslate.x = nextX;
        config.characterTranslate.z = nextZ;
    }
    // if blocked, try X only (slide along Z wall)
    else if (isTileWalkable(nextX, config.characterTranslate.z))
    {
        config.characterTranslate.x = nextX;
    }
    // if blocked, try Z only (slide along X wall)
    else if (isTileWalkable(config.characterTranslate.x, nextZ))
    {
        config.characterTranslate.z = nextZ;
    }

    if (deltaX != 0.0f || deltaZ != 0.0f)
    {
        // return angle in radians and convert to degrees
        angleRad = math.atan2(deltaZ, deltaX);
        config.characterFacingAngle = angleRad * (180.0f / 3.14159265f);
    }
}

void MovementComponent::configureMovement(const MovementConfig& config)
{
    this->config = config;
}

void MovementComponent::start()
{
    isActive = true;
}

void MovementComponent::stop()
{
    isActive = false;
}

CharacterPosition MovementComponent::isCharacterAt()
{
    CharacterPosition charPos;

    charPos.x = config.characterTranslate.x;
    charPos.z = config.characterTranslate.z;
    charPos.y = config.height;
    charPos.facingAngle = config.characterFacingAngle;

    return charPos;
}

TileType MovementComponent::isTileAt()
{
    int tileX = (int)((config.characterTranslate.x + config.worldOffsetX) / config.tileSize);
    int tileZ = (int)((config.characterTranslate.z + config.worldOffsetZ) / config.tileSize);
    return isTileAt(tileX, tileZ);
}

TileType MovementComponent::isTileAt(int tileX, int tileZ)
{
    // default
    if (tileX < 0 || tileX >= config.mapWidth || tileZ < 0 || tileZ >= config.mapHeight)
        return TileType::NO_COLLISION;

    // else use collision data
    return (TileType)config.collisionMap[(tileZ * config.mapWidth) + tileX];
}

bool MovementComponent::isTileWalkable(float worldX, float worldZ)
{
    float distanceToEdge = config.characterSize.x * 0.5f;

    int tileMinX = (int)((worldX - distanceToEdge + config.worldOffsetX) / config.tileSize);
    int tileMaxX = (int)((worldX + distanceToEdge + config.worldOffsetX) / config.tileSize);
    int tileMinZ = (int)((worldZ - distanceToEdge + config.worldOffsetZ) / config.tileSize);
    int tileMaxZ = (int)((worldZ + distanceToEdge + config.worldOffsetZ) / config.tileSize);

    for (int z = tileMinZ; z <= tileMaxZ; z++)
    {
        for (int x = tileMinX; x <= tileMaxX; x++)
        {
            if (isTileAt(x, z) == TileType::COLLISION)
            {
                return false;
            }
        }
    }

    return true;
}
