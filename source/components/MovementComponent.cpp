#include "MovementComponent.hpp"
#include "core/globals.hpp"

// TODO: remove hardcoded model references
#include "models/makoto.hpp"

// TODO: decouple camera with movement (so that movement can be used by other entities) (make a seperate component?)
// TODO: decouple animation controller with movement (make a seperate component?)

void MovementComponent::Init()
{
    isActive = false;
    walkAnim = (int)MODEL_MAKOTO_PLAYER_ROOT_MODEL_MOTION_0002;
    idleAnim = (int)MODEL_MAKOTO_PLAYER_ROOT_MODEL_MOTION;
}

void MovementComponent::Destroy()
{
    isActive = false;
}

void MovementComponent::Update(ae::q20_12_t)
{
    // TODO: don't broadcast on every update (waste cpu cycles). Set it once?
    ae::BroadcastEvent(Event::SetCharacterPosition{isCharacterAt()});
    ae::q20_12_t cameraAngle = CameraSystem::GetInstance().getMovementAngle();
    ae::q20_12_t forwardX;
    ae::q20_12_t forwardZ;
    ae::q20_12_t rightX;
    ae::q20_12_t rightZ;

    ae::q20_12_t deltaX{0};
    ae::q20_12_t deltaZ{0};

    ae::q20_12_t nextX;
    ae::q20_12_t nextZ;

    ae::q20_12_t angleRad;

    const ae::q20_12_t sinVal = ae::q20_12_t{math.sin(cameraAngle)};
    const ae::q20_12_t cosVal = ae::q20_12_t{math.cos(cameraAngle)};

    forwardX = -sinVal * config.speed;
    forwardZ = cosVal * config.speed;
    rightX = cosVal * config.speed;
    rightZ = sinVal * config.speed;

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

    if (deltaX != ae::q20_12_t{0} || deltaZ != ae::q20_12_t{0})
    {
        // set walking animation
        if (Globals::enableCharacterAnim && (animationCtrl->getCurrentAnimIndex() != walkAnim))
        {
            animationCtrl->set(walkAnim, true);
        }

        // normalize diagonal movement to prevent faster speed
        if (deltaX != ae::q20_12_t{0} && deltaZ != ae::q20_12_t{0})
        {
            const ae::q20_12_t invSqrt2{0.707106781187};
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

    if (deltaX != ae::q20_12_t{0} || deltaZ != ae::q20_12_t{0})
    {
        angleRad = math.atan2(deltaX, deltaZ);
        config.characterFacingAngle = angleRad * math.div(ae::q20_12_t{180}, ae::q20_12_t{3.14159265});
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
    MathManager& math = MathManager::GetInstance();

    int tileX = (int)(math.div(config.characterTranslate.x + config.worldOffsetX, config.tileSize));
    int tileZ = (int)(math.div(config.characterTranslate.z + config.worldOffsetZ, config.tileSize));
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

bool MovementComponent::isTileWalkable(ae::q20_12_t worldX, ae::q20_12_t worldZ)
{
    ae::q20_12_t distanceToEdge = config.characterSize.x * ae::q20_12_t{0.5};

    int tileMinX = static_cast<int>(math.div(worldX - distanceToEdge + config.worldOffsetX, config.tileSize));
    int tileMaxX = static_cast<int>(math.div(worldX + distanceToEdge + config.worldOffsetX, config.tileSize));
    int tileMinZ = static_cast<int>(math.div(worldZ - distanceToEdge + config.worldOffsetZ, config.tileSize));
    int tileMaxZ = static_cast<int>(math.div(worldZ + distanceToEdge + config.worldOffsetZ, config.tileSize));

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
