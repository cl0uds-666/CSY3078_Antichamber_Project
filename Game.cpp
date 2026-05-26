#include "Game.h"
#include <Windows.h>
#include <cmath>
#include <sstream>

namespace
{
const int CorridorLoopCollectibleIndex = 0;
const int LeftRoomCollectibleIndex = 1;
const int Room3CollectibleIndex = 2;

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
    mLeftRoomCollectibleUnlocked = false;
    mRoom3CollectibleUnlocked = false;

    mCollectedCount = 0;
    mHasPreviousPlayerPosition = false;

    mRoom2CanTrigger = true;
    mRoom3State = Room3State::Normal;
    mRoom3CanTrigger = true;
    mRoom3LookAwayFrameCount = 0;
    mRoom3LookAwayCooldownFrames = 0;
    ApplyRoom3Layout(mRoom3State);

    Collectible corridorLoopCollectible;
    // First pickup appears in plain sight after loop condition is met.
    corridorLoopCollectible.position = XMFLOAT3(0.0f, -0.2f, 17.2f);
    corridorLoopCollectible.isSpawned = false;
    corridorLoopCollectible.isCollected = false;

    Collectible leftRoomCollectible;
    // Second pickup sits near the left-room return route.
    leftRoomCollectible.position = XMFLOAT3(1.2f, -0.2f, 13.8f);
    leftRoomCollectible.isSpawned = false;
    leftRoomCollectible.isCollected = false;

    Collectible room3Collectible;
    // Final pickup sits between room 3 and the corridor loop trigger.
    room3Collectible.position = XMFLOAT3(-1.2f, -0.2f, 18.2f);
    room3Collectible.isSpawned = false;
    room3Collectible.isCollected = false;

    mCollectibles.push_back(corridorLoopCollectible);
    mCollectibles.push_back(leftRoomCollectible);
    mCollectibles.push_back(room3Collectible);
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

    // Keep corridor looping limited to the hallway lane.
    bool isInsideMainCorridor =
        playerPosition.x >= -2.0f &&
        playerPosition.x <= 2.0f;

    // Trigger before the visible end of the corridor
    if (isInsideMainCorridor && playerPosition.z > 20.0f)
    {
        mLoopCount++;

        // Snap back to the early corridor segment.
        camera.SetPosition(XMFLOAT3(
            playerPosition.x,
            playerPosition.y,
            2.0f));

        // Third loop unlocks collectible #1.
        if (mLoopCount >= 3 && !mFirstCollectibleUnlocked)
        {
            mFirstCollectibleUnlocked = true;
            mCollectibles[CorridorLoopCollectibleIndex].isSpawned = true;
        }
    }
}

void Game::CheckRoom2Illusion(Camera& camera)
{
    XMFLOAT3 playerPosition = camera.GetPosition();

    // Left room has two fake exits; only exits teleport.
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
        // Back fake exit returns player facing right.
        camera.SetPosition(XMFLOAT3(-0.2f, 0.0f, 10.0f));
        camera.AddYaw(-XM_PIDIV2);

        if (!mLeftRoomCollectibleUnlocked)
        {
            mLeftRoomCollectibleUnlocked = true;
            mCollectibles[LeftRoomCollectibleIndex].isSpawned = true;
        }

        mRoom2CanTrigger = false;
        return;
    }

    if (IsInsideTrigger(playerPosition, fakeExitBCentre, exitHalfSize))
    {
        // Front fake exit returns player facing left.
        camera.SetPosition(XMFLOAT3(-0.2f, 0.0f, 10.0f));
        camera.AddYaw(XM_PIDIV2);

        if (!mLeftRoomCollectibleUnlocked)
        {
            mLeftRoomCollectibleUnlocked = true;
            mCollectibles[LeftRoomCollectibleIndex].isSpawned = true;
        }

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

    // Whole right room is an active zone for the look-away illusion.
    XMFLOAT3 activeZoneCentre = XMFLOAT3(7.1f, 0.0f, 18.0f);
    XMFLOAT3 activeZoneHalfSize = XMFLOAT3(4.5f, 1.0f, 2.6f);

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

    XMFLOAT3 activeDoorAnchor = XMFLOAT3(2.2f, 0.0f, 19.0f);

    if (mRoom3State == Room3State::ShiftedA)
    {
        activeDoorAnchor = XMFLOAT3(7.1f, 0.0f, 21.0f);
    }
    else if (mRoom3State == Room3State::ShiftedB)
    {
        activeDoorAnchor = XMFLOAT3(7.1f, 0.0f, 15.0f);
    }

    XMFLOAT3 forwardDirection = camera.GetForwardDirection();

    float anchorX = activeDoorAnchor.x - playerPosition.x;
    float anchorZ = activeDoorAnchor.z - playerPosition.z;
    float anchorLength = sqrtf(anchorX * anchorX + anchorZ * anchorZ);

    if (anchorLength <= 0.001f)
    {
        mRoom3LookAwayFrameCount = 0;
        return;
    }

    float anchorDirectionX = anchorX / anchorLength;
    float anchorDirectionZ = anchorZ / anchorLength;
    float activeDoorDot =
        forwardDirection.x * anchorDirectionX +
        forwardDirection.z * anchorDirectionZ;

    // Look at the active door to arm it, then look away to shift layout.
    float lookAtDotThreshold = 0.35f;
    float lookAwayDotThreshold = -0.05f;
    int requiredLookAwayFrames = 18;
    int cooldownFrames = 30;

    bool isLookingAtActiveDoor = activeDoorDot > lookAtDotThreshold;
    bool isLookingAwayFromActiveDoor = activeDoorDot < lookAwayDotThreshold;

    if (mRoom3LookAwayCooldownFrames > 0)
    {
        mRoom3LookAwayCooldownFrames--;
        mRoom3LookAwayFrameCount = 0;
        return;
    }

    if (!mRoom3CanTrigger)
    {
        if (isLookingAtActiveDoor)
        {
            mRoom3CanTrigger = true;
        }

        mRoom3LookAwayFrameCount = 0;
        return;
    }

    if (!isLookingAwayFromActiveDoor)
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

    if (previousState == Room3State::ShiftedB &&
        mRoom3State == Room3State::Normal &&
        !mRoom3CollectibleUnlocked)
    {
        mRoom3CollectibleUnlocked = true;
        mCollectibles[Room3CollectibleIndex].isSpawned = true;
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
        << "ActiveDoorDot=" << activeDoorDot << "\n";
    Room3DebugLog(stream.str());
}

void Game::ApplyRoom3Layout(Room3State state)
{
    mRoom3LayoutProps.clear();

    // Dynamic slabs close inactive doorways so one doorway stays truly open.
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

    // Small puddle marks the wall where the doorway just moved from.
    SceneObject meltedDoorPuddle;
    meltedDoorPuddle.scale = XMFLOAT3(1.0f, 0.05f, 0.55f);

    if (state == Room3State::Normal)
    {
        // Default state: corridor door open, wall doors sealed.
        sideBlock.position = XMFLOAT3(9.8f, 0.0f, 19.2f);
        meltedDoorPuddle.position = XMFLOAT3(2.55f, -0.9f, 19.0f);

        mRoom3LayoutProps.push_back(backDoorSlab);
        mRoom3LayoutProps.push_back(frontDoorSlab);
    }
    else if (state == Room3State::ShiftedA)
    {
        // ShiftedA: front wall door open.
        sideBlock.position = XMFLOAT3(6.2f, 0.0f, 17.4f);
        meltedDoorPuddle.position = XMFLOAT3(7.1f, -0.9f, 20.65f);

        mRoom3LayoutProps.push_back(corridorDoorSlab);
        mRoom3LayoutProps.push_back(backDoorSlab);
    }
    else
    {
        // ShiftedB: back wall door open.
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
