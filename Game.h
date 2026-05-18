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

enum class Room3State
{
    Normal,
    ShiftedA,
    ShiftedB
};

class Game
{
public:

    Game();

    void SetLevel(const Level* level);

    void Update(Camera& camera);

    const std::vector<Collectible>& GetCollectibles() const;
    const std::vector<SceneObject>& GetRoom3LayoutProps() const;

    int GetCollectedCount() const;

private:

    void CheckLoopingCorridor(Camera& camera);

    void CheckRoom2Illusion(Camera& camera);

    void CheckRoom3Illusion(Camera& camera);
    void CheckRoom3LookAwayToggle(Camera& camera);
    void ApplyRoom3Layout(Room3State state);

    void UpdateCollectibles(Camera& camera);

    void ResolveCollision(Camera& camera);

    bool IsInsideTrigger(
        XMFLOAT3 position,
        XMFLOAT3 centre,
        XMFLOAT3 halfSize) const;

private:

    const Level* mLevel;

    int mLoopCount;
    bool mFirstCollectibleUnlocked;
    bool mLeftRoomCollectibleUnlocked;
    bool mRoom3CollectibleUnlocked;

    std::vector<Collectible> mCollectibles;
    int mCollectedCount;

    XMFLOAT3 mPreviousPlayerPosition;
    bool mHasPreviousPlayerPosition;

    bool mRoom2CanTrigger;

    Room3State mRoom3State;
    bool mRoom3CanTrigger;
    int mRoom3LookAwayFrameCount;
    int mRoom3LookAwayCooldownFrames;

    std::vector<SceneObject> mRoom3LayoutProps;
};
