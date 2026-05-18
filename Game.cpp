#include "Game.h"
#include <Windows.h>
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
    CheckRoom3Illusion(camera);

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
    XMFLOAT3 playerPosition = camera.GetPosition();
    Room3State previousState = mRoom3State;

    // Tuned to observed playable right-room area from debug traces:
    // approximately x in [5.5, 10.8], z in [15.5, 20.0].
    // We allow transitions in:
    //  - a central room zone
    //  - a connector/front zone near room entrance.
    XMFLOAT3 rightRoomCentreTrigger = XMFLOAT3(7.1f, 0.0f, 18.7f);
    XMFLOAT3 rightRoomCentreHalfSize = XMFLOAT3(1.8f, 1.0f, 1.2f);

    XMFLOAT3 connectorTrigger = XMFLOAT3(7.1f, 0.0f, 16.6f);
    XMFLOAT3 connectorHalfSize = XMFLOAT3(1.6f, 1.0f, 1.0f);

    bool insideCentre =
        IsInsideTrigger(playerPosition, rightRoomCentreTrigger, rightRoomCentreHalfSize);
    bool insideConnector =
        IsInsideTrigger(playerPosition, connectorTrigger, connectorHalfSize);

    bool insideAnyTrigger = insideCentre || insideConnector;
    static bool wasInsideAnyTrigger = false;
    static int debugFrameCounter = 0;

    debugFrameCounter++;
    if (debugFrameCounter >= 120)
    {
        std::ostringstream stream;
        stream
            << "[Room3 Debug] Pos("
            << playerPosition.x << ", "
            << playerPosition.y << ", "
            << playerPosition.z << ") "
            << "State=" << ToRoom3StateString(mRoom3State) << " "
            << "CanTrigger=" << (mRoom3CanTrigger ? "true" : "false") << " "
            << "InsideCentre=" << (insideCentre ? "true" : "false") << " "
            << "InsideConnector=" << (insideConnector ? "true" : "false")
            << "\n";
        Room3DebugLog(stream.str());
        debugFrameCounter = 0;
    }

    if (insideAnyTrigger != wasInsideAnyTrigger)
    {
        std::ostringstream stream;
        stream
            << "[Room3 Debug] Trigger boundary crossed. "
            << "insideAnyTrigger=" << (insideAnyTrigger ? "true" : "false") << " "
            << "Pos(" << playerPosition.x << ", "
            << playerPosition.y << ", "
            << playerPosition.z << ")\n";
        Room3DebugLog(stream.str());
        wasInsideAnyTrigger = insideAnyTrigger;
    }

    if (!mRoom3CanTrigger)
    {
        if (!insideAnyTrigger)
        {
            mRoom3CanTrigger = true;
        }

        return;
    }

    if (!insideAnyTrigger)
    {
        return;
    }

    XMFLOAT3 shiftedPosition = playerPosition;

    if (insideCentre)
    {
        if (mRoom3State == Room3State::Normal)
        {
            mRoom3State = Room3State::ShiftedA;
            shiftedPosition.x = 8.6f;
            shiftedPosition.z = 19.3f;
        }
        else if (mRoom3State == Room3State::ShiftedA)
        {
            mRoom3State = Room3State::ShiftedB;
            shiftedPosition.x = 5.8f;
            shiftedPosition.z = 18.4f;
        }
        else
        {
            mRoom3State = Room3State::Normal;
            shiftedPosition.x = 7.1f;
            shiftedPosition.z = 18.7f;
        }
    }
    else if (insideConnector)
    {
        // Connector zone swaps between shifted layouts.
        if (mRoom3State == Room3State::ShiftedA)
        {
            mRoom3State = Room3State::ShiftedB;
            shiftedPosition.x = 6.0f;
            shiftedPosition.z = 17.5f;
        }
        else if (mRoom3State == Room3State::ShiftedB)
        {
            mRoom3State = Room3State::ShiftedA;
            shiftedPosition.x = 8.3f;
            shiftedPosition.z = 18.0f;
        }
        else
        {
            // First connector hit from normal state nudges into ShiftedA.
            mRoom3State = Room3State::ShiftedA;
            shiftedPosition.x = 8.0f;
            shiftedPosition.z = 17.2f;
        }
    }

    camera.SetPosition(shiftedPosition);
    mRoom3CanTrigger = false;
    ApplyRoom3Layout(mRoom3State);

    std::ostringstream stream;
    stream
        << "[Room3 Debug] Transition fired. "
        << ToRoom3StateString(previousState) << " -> "
        << ToRoom3StateString(mRoom3State) << " "
        << "NewPos(" << shiftedPosition.x << ", "
        << shiftedPosition.y << ", "
        << shiftedPosition.z << ")\n";
    Room3DebugLog(stream.str());
}

void Game::ApplyRoom3Layout(Room3State state)
{
    mRoom3LayoutProps.clear();

    // Two doorway lanes in validated right-room playable bounds.
    // Doorway A = left lane, Doorway B = right lane.
    XMFLOAT3 doorwayA = XMFLOAT3(5.9f, 0.0f, 18.6f);
    XMFLOAT3 doorwayB = XMFLOAT3(8.9f, 0.0f, 18.6f);

    SceneObject centralPillar;
    centralPillar.position = XMFLOAT3(7.1f, 0.0f, 18.2f);
    centralPillar.scale = XMFLOAT3(0.45f, 1.8f, 0.45f);
    mRoom3LayoutProps.push_back(centralPillar);

    SceneObject doorwayBlocker;
    doorwayBlocker.scale = XMFLOAT3(0.55f, 1.4f, 0.55f);

    SceneObject sideBlock;
    sideBlock.scale = XMFLOAT3(0.9f, 1.0f, 0.4f);

    // Visual doorway "wall cap" to make the active doorway appear to move.
    // This is a tall thin panel that closes one lane while the other remains open.
    SceneObject doorwayWallCap;
    doorwayWallCap.scale = XMFLOAT3(0.35f, 1.8f, 0.25f);

    // Big visual "moving door wall" panel so the doorway appears to move
    // between different room walls (front/back), not just side-to-side.
    SceneObject movingDoorWall;
    movingDoorWall.scale = XMFLOAT3(1.3f, 1.9f, 0.2f);

    if (state == Room3State::Normal)
    {
        // Neutral baseline: centre blocker, both doorway lanes still readable.
        doorwayBlocker.position = XMFLOAT3(7.1f, 0.0f, 16.9f);
        sideBlock.position = XMFLOAT3(9.8f, 0.0f, 19.2f);
        doorwayWallCap.position = XMFLOAT3(7.1f, 0.0f, 18.6f);
        movingDoorWall.position = XMFLOAT3(7.1f, 0.0f, 20.8f);
    }
    else if (state == Room3State::ShiftedA)
    {
        // ShiftedA: block doorway B, keep doorway A open.
        doorwayBlocker.position = doorwayB;
        sideBlock.position = XMFLOAT3(6.2f, 0.0f, 17.4f);
        doorwayWallCap.position = doorwayB;
        // Door looks moved to the front wall in this state.
        movingDoorWall.position = XMFLOAT3(7.1f, 0.0f, 20.8f);
    }
    else
    {
        // ShiftedB: block doorway A, keep doorway B open.
        doorwayBlocker.position = doorwayA;
        sideBlock.position = XMFLOAT3(8.6f, 0.0f, 17.4f);
        doorwayWallCap.position = doorwayA;
        // Door looks moved to the back wall in this state.
        movingDoorWall.position = XMFLOAT3(7.1f, 0.0f, 15.2f);
    }

    mRoom3LayoutProps.push_back(doorwayBlocker);
    mRoom3LayoutProps.push_back(sideBlock);
    mRoom3LayoutProps.push_back(doorwayWallCap);
    mRoom3LayoutProps.push_back(movingDoorWall);
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
