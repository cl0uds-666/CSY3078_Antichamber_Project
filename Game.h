#pragma once

#include <vector>
#include <DirectXMath.h>
#include "Camera.h"
#include "Level.h"

using namespace DirectX;

struct Collectible
{
    XMFLOAT3 position;
    bool isSpawned;
    bool isCollected;
};

class Game
{
public:

    Game();

    void SetLevel(const Level* level);

    void Update(Camera& camera);

    const std::vector<Collectible>& GetCollectibles() const;

    int GetCollectedCount() const;

private:

    void CheckLoopingCorridor(Camera& camera);

    void UpdateCollectibles(Camera& camera);

    void ResolveCollision(Camera& camera);

private:

    const Level* mLevel;

    int mLoopCount;
    bool mFirstCollectibleUnlocked;

    std::vector<Collectible> mCollectibles;
    int mCollectedCount;

    XMFLOAT3 mPreviousPlayerPosition;
    bool mHasPreviousPlayerPosition;
};
