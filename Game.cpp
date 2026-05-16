#include "Game.h"

Game::Game()
{
    mLevel = nullptr;

    mLoopCount = 0;
    mFirstCollectibleUnlocked = false;

    mCollectedCount = 0;
    mHasPreviousPlayerPosition = false;

    Collectible firstCollectible;
    firstCollectible.position = XMFLOAT3(-9.5f, -0.2f, 10.0f);
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

void Game::CheckLoopingCorridor(Camera& camera)
{
    XMFLOAT3 playerPosition = camera.GetPosition();

    // Trigger before the visible end of the corridor
    if (playerPosition.z > 20.0f)
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
}
