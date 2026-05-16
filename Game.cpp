#include "Game.h"

Game::Game()
{
    mLoopCount = 0;
    mFirstCollectibleUnlocked = false;
}

void Game::Update(Camera& camera)
{
    CheckLoopingCorridor(camera);
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
        }
    }
}
