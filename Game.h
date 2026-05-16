#pragma once

#include "Camera.h"

class Game
{
public:

    Game();

    void Update(Camera& camera);

private:

    void CheckLoopingCorridor(Camera& camera);

private:

    int mLoopCount;
    bool mFirstCollectibleUnlocked;
};