#include "Game.h"
#include <Windows.h>
#include <cmath>
#include <sstream>

namespace
{
const char* ToRoom3StateString(Room3State state)
{
    if (state == Room3State::Normal)
    {
        return "Normal";
    }

    if (state == Room3State::ShiftedA)
    {
        return "ShiftedA";
    }

    return "ShiftedB";
}

void Room3DebugLog(const std::string& message)
{
    OutputDebugStringA(message.c_str());
}
}

Game::Game()
{
    mLevel = nullptr;

    mLoopCount = 0;
    mFirstCollectibleUnlocked = false;

    mCollectedCount = 0;
    mHasPreviousPlayerPosition = false;

    mRoom2CanTrigger = true;
    mRoom3State = Room3State::Normal;
    mRoom3CanTrigger = true;
    mRoom3LookAwayFrameCount = 0;
    mRoom3LookAwayCooldownFrames = 0;
    ApplyRoom3Layout(mRoom3State);

    Collectible firstCollectible;
    // Spawn in the main corridor so it is clearly visible after unlock.
    // Keep this away from room filler geometry to avoid hidden pickups.
    firstCollectible.position = XMFLOAT3(0.0f, -0.2f, 19.0f);
    firstCollectible.isSpawned = false;
    firstCollectible.isCollected = false;

    mCollectibles.push_back(firstCollectible);
}

void Game::SetLevel(const Level* level)
{
    mLevel = level;
}

void Game::Update(Camera& camera)
{
    if (!mHasPreviousPlayerPosition)
    {
        mPreviousPlayerPosition = camera.GetPosition();
        mHasPreviousPlayerPosition = true;
    }

    CheckLoopingCorridor(camera);

    CheckRoom2Illusion(camera);
    CheckRoom3LookAwayToggle(camera);

    ResolveCollision(camera);

    UpdateCollectibles(camera);

    mPreviousPlayerPosition = camera.GetPosition();
}

const std::vector<Collectible>& Game::GetCollectibles() const
{
    return mCollectibles;
}

int Game::GetCollectedCount() const
{
    return mCollectedCount;
}

const std::vector<SceneObject>& Game::GetRoom3LayoutProps() const
{
    return mRoom3LayoutProps;
}

void Game::CheckLoopingCorridor(Camera& camera)
{
    XMFLOAT3 playerPosition = camera.GetPosition();

    // Only trigger the corridor loop when the player is still in the
    // main corridor lane. This keeps the right-hand room free of
    // teleportation behaviour.
    bool isInsideMainCorridor =
        playerPosition.x >= -2.0f &&
        playerPosition.x <= 2.0f;

    // Trigger before the visible end of the corridor
    if (isInsideMainCorridor && playerPosition.z > 20.0f)
    {
        mLoopCount++;

        // Send player back to an earlier part of the same corridor
        camera.SetPosition(XMFLOAT3(
            playerPosition.x,
            playerPosition.y,
            2.0f));

        // After 3 loops, unlock the first collectible
        if (mLoopCount >= 3)
        {
            mFirstCollectibleUnlocked = true;
            mCollectibles[0].isSpawned = true;
        }
    }
}

void Game::CheckRoom2Illusion(Camera& camera)
{
    XMFLOAT3 playerPosition = camera.GetPosition();

    // Two fake exit corridors inside the left room.
    // Entering the room from corridor is fully normal (no entrance teleport).
    XMFLOAT3 fakeExitACentre = XMFLOAT3(-9.0f, 0.0f, 6.0f);
    XMFLOAT3 fakeExitBCentre = XMFLOAT3(-9.0f, 0.0f, 14.0f);
    XMFLOAT3 exitHalfSize = XMFLOAT3(1.2f, 1.0f, 1.2f);

    bool insideAnyTrigger =
        IsInsideTrigger(playerPosition, fakeExitACentre, exitHalfSize) ||
        IsInsideTrigger(playerPosition, fakeExitBCentre, exitHalfSize);

    if (!mRoom2CanTrigger)
    {
        if (!insideAnyTrigger)
        {
            mRoom2CanTrigger = true;
        }

        return;
    }

    if (IsInsideTrigger(playerPosition, fakeExitACentre, exitHalfSize))
    {
        // Back-side fake exit: return to corridor and turn right (90 deg).
        camera.SetPosition(XMFLOAT3(-0.2f, 0.0f, 10.0f));
        camera.AddYaw(-XM_PIDIV2);
        mRoom2CanTrigger = false;
        return;
    }

    if (IsInsideTrigger(playerPosition, fakeExitBCentre, exitHalfSize))
    {
        // Front-side fake exit: return to corridor and turn left (-90 deg).
        camera.SetPosition(XMFLOAT3(-0.2f, 0.0f, 10.0f));
        camera.AddYaw(XM_PIDIV2);
        mRoom2CanTrigger = false;
        return;
    }
}

void Game::CheckRoom3Illusion(Camera& camera)
{
    CheckRoom3LookAwayToggle(camera);
}

void Game::CheckRoom3LookAwayToggle(Camera& camera)
{
    XMFLOAT3 playerPosition = camera.GetPosition();

    XMFLOAT3 activeZoneCentre = XMFLOAT3(7.1f, 0.0f, 18.0f);
    XMFLOAT3 activeZoneHalfSize = XMFLOAT3(3.0f, 1.0f, 3.0f);

    bool insideActiveZone = IsInsideTrigger(
        playerPosition,
        activeZoneCentre,
        activeZoneHalfSize);

    if (!insideActiveZone)
    {
        mRoom3CanTrigger = true;
        mRoom3LookAwayFrameCount = 0;
        mRoom3LookAwayCooldownFrames = 0;
        return;
    }

    XMFLOAT3 forwardDirection = camera.GetForwardDirection();

    XMFLOAT3 layoutAnchors[3] =
    {
        XMFLOAT3(2.2f, 0.0f, 19.0f),
        XMFLOAT3(7.1f, 0.0f, 15.0f),
        XMFLOAT3(7.1f, 0.0f, 21.0f)
    };

    float bestAnchorDot = -1.0f;

    for (int i = 0; i < 3; i++)
    {
        float anchorX = layoutAnchors[i].x - playerPosition.x;
        float anchorZ = layoutAnchors[i].z - playerPosition.z;
        float anchorLength = sqrtf(anchorX * anchorX + anchorZ * anchorZ);

        if (anchorLength <= 0.001f)
        {
            continue;
        }

        float anchorDirectionX = anchorX / anchorLength;
        float anchorDirectionZ = anchorZ / anchorLength;
        float anchorDot =
            forwardDirection.x * anchorDirectionX +
            forwardDirection.z * anchorDirectionZ;

        if (anchorDot > bestAnchorDot)
        {
            bestAnchorDot = anchorDot;
        }
    }

    // Dot below this value means the player is at least about 80 degrees away
    // from every key doorway anchor, so the door is safely out of view.
    float lookAwayDotThreshold = 0.17f;
    int requiredLookAwayFrames = 24;
    int cooldownFrames = 45;

    bool isLookingAway = bestAnchorDot < lookAwayDotThreshold;

    if (mRoom3LookAwayCooldownFrames > 0)
    {
        mRoom3LookAwayCooldownFrames--;
        mRoom3LookAwayFrameCount = 0;
        return;
    }

    if (!mRoom3CanTrigger)
    {
        if (!isLookingAway)
        {
            mRoom3CanTrigger = true;
        }

        mRoom3LookAwayFrameCount = 0;
        return;
    }

    if (!isLookingAway)
    {
        mRoom3LookAwayFrameCount = 0;
        return;
    }

    mRoom3LookAwayFrameCount++;

    if (mRoom3LookAwayFrameCount < requiredLookAwayFrames)
    {
        return;
    }

    Room3State previousState = mRoom3State;

    if (mRoom3State == Room3State::Normal)
    {
        mRoom3State = Room3State::ShiftedA;
    }
    else if (mRoom3State == Room3State::ShiftedA)
    {
        mRoom3State = Room3State::ShiftedB;
    }
    else
    {
        mRoom3State = Room3State::Normal;
    }

    mRoom3CanTrigger = false;
    mRoom3LookAwayFrameCount = 0;
    mRoom3LookAwayCooldownFrames = cooldownFrames;
    ApplyRoom3Layout(mRoom3State);

    std::ostringstream stream;
    stream
        << "[Room3 Debug] Look-away transition fired. "
        << ToRoom3StateString(previousState) << " -> "
        << ToRoom3StateString(mRoom3State) << " "
        << "BestAnchorDot=" << bestAnchorDot << "\n";
    Room3DebugLog(stream.str());
}

void Game::ApplyRoom3Layout(Room3State state)
{
    mRoom3LayoutProps.clear();

    // Room 3 now treats the corridor-room entrance as the door that melts
    // and re-forms on different walls. The static level leaves real doorway
    // gaps in each candidate wall; these dynamic slabs seal every inactive
    // doorway, so the active state is an actual opening rather than a block
    // stuck on top of a wall.
    XMFLOAT3 originalCorridorDoor = XMFLOAT3(2.2f, 0.0f, 19.0f);
    XMFLOAT3 backWallDoor = XMFLOAT3(7.1f, 0.0f, 15.0f);
    XMFLOAT3 frontWallDoor = XMFLOAT3(7.1f, 0.0f, 21.0f);

    SceneObject centralPillar;
    centralPillar.position = XMFLOAT3(7.1f, 0.0f, 18.2f);
    centralPillar.scale = XMFLOAT3(0.45f, 1.8f, 0.45f);
    mRoom3LayoutProps.push_back(centralPillar);

    SceneObject sideBlock;
    sideBlock.scale = XMFLOAT3(0.9f, 1.0f, 0.4f);

    SceneObject corridorDoorSlab;
    corridorDoorSlab.position = originalCorridorDoor;
    corridorDoorSlab.scale = XMFLOAT3(0.2f, 2.0f, 2.0f);

    SceneObject backDoorSlab;
    backDoorSlab.position = backWallDoor;
    backDoorSlab.scale = XMFLOAT3(1.2f, 2.0f, 0.2f);

    SceneObject frontDoorSlab;
    frontDoorSlab.position = frontWallDoor;
    frontDoorSlab.scale = XMFLOAT3(1.2f, 2.0f, 0.2f);

    // A shallow floor-level puddle marks the doorway that has just melted.
    // It sits below the player collision bounds, so it is purely visual.
    SceneObject meltedDoorPuddle;
    meltedDoorPuddle.scale = XMFLOAT3(1.0f, 0.05f, 0.55f);

    if (state == Room3State::Normal)
    {
        // Original layout: the corridor-to-room entrance remains exactly
        // where the player first found it, with the other wall doors sealed.
        sideBlock.position = XMFLOAT3(9.8f, 0.0f, 19.2f);
        meltedDoorPuddle.position = XMFLOAT3(2.55f, -0.9f, 19.0f);

        mRoom3LayoutProps.push_back(backDoorSlab);
        mRoom3LayoutProps.push_back(frontDoorSlab);
    }
    else if (state == Room3State::ShiftedA)
    {
        // ShiftedA: the original corridor doorway has re-formed as a real
        // opening on the front wall, so the corridor and back wall are closed.
        sideBlock.position = XMFLOAT3(6.2f, 0.0f, 17.4f);
        meltedDoorPuddle.position = XMFLOAT3(7.1f, -0.9f, 20.65f);

        mRoom3LayoutProps.push_back(corridorDoorSlab);
        mRoom3LayoutProps.push_back(backDoorSlab);
    }
    else
    {
        // ShiftedB: the same door has melted into the back wall instead.
        sideBlock.position = XMFLOAT3(8.6f, 0.0f, 17.4f);
        meltedDoorPuddle.position = XMFLOAT3(7.1f, -0.9f, 15.35f);

        mRoom3LayoutProps.push_back(corridorDoorSlab);
        mRoom3LayoutProps.push_back(frontDoorSlab);
    }

    mRoom3LayoutProps.push_back(sideBlock);
    mRoom3LayoutProps.push_back(meltedDoorPuddle);
}

void Game::UpdateCollectibles(Camera& camera)
{
    XMFLOAT3 playerPosition = camera.GetPosition();

    for (int i = 0; i < mCollectibles.size(); i++)
    {
        Collectible& collectible = mCollectibles[i];

        if (!collectible.isSpawned || collectible.isCollected)
        {
            continue;
        }

        float dx = playerPosition.x - collectible.position.x;
        float dy = playerPosition.y - collectible.position.y;
        float dz = playerPosition.z - collectible.position.z;

        float distanceSquared = dx * dx + dy * dy + dz * dz;

        if (distanceSquared < 1.0f)
        {
            collectible.isCollected = true;
            mCollectedCount++;
        }
    }
}

void Game::ResolveCollision(Camera& camera)
{
    if (mLevel == nullptr)
    {
        return;
    }

    XMFLOAT3 playerPosition = camera.GetPosition();

    const std::vector<SceneObject>& sceneObjects = mLevel->GetSceneObjects();
    const std::vector<SceneObject>& room3LayoutProps = mRoom3LayoutProps;

    float playerHalfWidth = 0.35f;
    float playerHalfHeight = 0.8f;

    for (int i = 0; i < sceneObjects.size(); i++)
    {
        SceneObject object = sceneObjects[i];

        float objectMinX = object.position.x - object.scale.x;
        float objectMaxX = object.position.x + object.scale.x;

        float objectMinY = object.position.y - object.scale.y;
        float objectMaxY = object.position.y + object.scale.y;

        float objectMinZ = object.position.z - object.scale.z;
        float objectMaxZ = object.position.z + object.scale.z;

        float playerMinX = playerPosition.x - playerHalfWidth;
        float playerMaxX = playerPosition.x + playerHalfWidth;

        float playerMinY = playerPosition.y - playerHalfHeight;
        float playerMaxY = playerPosition.y + playerHalfHeight;

        float playerMinZ = playerPosition.z - playerHalfWidth;
        float playerMaxZ = playerPosition.z + playerHalfWidth;

        bool overlapX = playerMaxX > objectMinX && playerMinX < objectMaxX;
        bool overlapY = playerMaxY > objectMinY && playerMinY < objectMaxY;
        bool overlapZ = playerMaxZ > objectMinZ && playerMinZ < objectMaxZ;

        if (overlapX && overlapY && overlapZ)
        {
            camera.SetPosition(mPreviousPlayerPosition);
            return;
        }
    }

    for (int i = 0; i < room3LayoutProps.size(); i++)
    {
        SceneObject object = room3LayoutProps[i];

        float objectMinX = object.position.x - object.scale.x;
        float objectMaxX = object.position.x + object.scale.x;

        float objectMinY = object.position.y - object.scale.y;
        float objectMaxY = object.position.y + object.scale.y;

        float objectMinZ = object.position.z - object.scale.z;
        float objectMaxZ = object.position.z + object.scale.z;

        float playerMinX = playerPosition.x - playerHalfWidth;
        float playerMaxX = playerPosition.x + playerHalfWidth;

        float playerMinY = playerPosition.y - playerHalfHeight;
        float playerMaxY = playerPosition.y + playerHalfHeight;

        float playerMinZ = playerPosition.z - playerHalfWidth;
        float playerMaxZ = playerPosition.z + playerHalfWidth;

        bool overlapX = playerMaxX > objectMinX && playerMinX < objectMaxX;
        bool overlapY = playerMaxY > objectMinY && playerMinY < objectMaxY;
        bool overlapZ = playerMaxZ > objectMinZ && playerMinZ < objectMaxZ;

        if (overlapX && overlapY && overlapZ)
        {
            camera.SetPosition(mPreviousPlayerPosition);
            return;
        }
    }
}

bool Game::IsInsideTrigger(
    XMFLOAT3 position,
    XMFLOAT3 centre,
    XMFLOAT3 halfSize) const
{
    bool insideX =
        position.x >= centre.x - halfSize.x &&
        position.x <= centre.x + halfSize.x;

    bool insideY =
        position.y >= centre.y - halfSize.y &&
        position.y <= centre.y + halfSize.y;

    bool insideZ =
        position.z >= centre.z - halfSize.z &&
        position.z <= centre.z + halfSize.z;

    return insideX && insideY && insideZ;
}
